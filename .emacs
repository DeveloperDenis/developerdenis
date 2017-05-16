; Based on the .emacs file of Casey Muratori

(require 'ido)


; DISABLE THE AWFUL ASM-MODE
(setq auto-mode-alist                        ; prepend following elts
      (nconc '(
               ("\\.asm$" . text-mode))
        auto-mode-alist))

; text style (for ASM)
(add-hook 'text-mode-hook
          '(lambda ()
             (setq tab-width 4)
	     (setq indent-tabs-mode t)
	     (setq fixme-modes 'text-mode)
	     (local-set-key (kbd "TAB") 'tab-to-tab-stop)))

;HOT KEYS
;(global-set-key (kbd "TAB") 'self-insert-command);

(define-key global-map "\ek" 'kill-this-buffer); ALT-k
(define-key global-map "\es" 'save-buffer) ; ALT-s
(define-key global-map "\ea" 'yank) ; ALT-a = paste
(define-key global-map "\ez" 'undo) ; ALT-u = undo
(define-key global-map "\ef" 'find-file)
(define-key global-map "\eF" 'find-file-other-window)
(define-key global-map "\en" 'next-error)
(define-key global-map "\eN" 'previous-error)
(define-key global-map [f9] 'first-error)
(define-key global-map [f10] 'previous-error)
(define-key global-map [f11] 'next-error)
(global-set-key (read-kbd-macro "\eb") 'ido-switch-buffer)
(global-set-key (read-kbd-macro "\eB") 'ido-switch-buffer-other-window)
(define-key global-map "\eo" 'query-replace)
(define-key global-map [f8] 'denis-replace-string)
(define-key global-map "\eO" 'denis-replace-string)
(define-key global-map "\el" 'denis-replace-in-region)
(define-key global-map "\eg" 'goto-line)

(set-variable 'grep-command "grep -irHn ")
(set-variable 'grep-command "findstr -s -n -i -l ")

(defun denis-ediff-setup-windows (buffer-A buffer-B buffer-C control-buffer)
  (ediff-setup-windows-plain buffer-A buffer-B buffer-C control-buffer)
)
(setq ediff-window-setup-function 'denis-ediff-setup-windows)
(setq ediff-split-window-function 'split-window-horizontally)

;set undo limit super high so we can undo FOR DAYS
(setq undo-limit 20000000)
(setq undo-strong-limit 40000000)

;turn off that darn alarm bell sound
(setq ring-bell-function 'ignore)

; I think this has to do with the middle line?
(global-hl-line-mode 1)
(set-face-background 'hl-line "midnight blue")
(scroll-bar-mode -1)
(setq denis-font "outline-DejaVu Sans Mono")

; this is turning off the ugly big top bar
(tool-bar-mode 0)

;get rid of word-wrapping/line-wrapping
(setq-default truncate-lines 1)

; Smooth scroll
(setq mouse-wheel-scroll-amount '(1 ((shift) . 1)))
(setq mouse-wheel-progressive-speed nil)
(setq mouse-wheel-follow-mouse 't)
(setq scroll-step 3)
(setq auto-window-vscroll nil)

; Set up font stuff
(add-to-list 'default-frame-alist '(font . "Liberation Mono-9"))
(set-face-attribute 'default t :font "Liberation Mono-9")
(set-face-attribute 'font-lock-builtin-face nil :foreground "#DAB98F")
(set-face-attribute 'font-lock-comment-face nil :foreground "gray50")
(set-face-attribute 'font-lock-constant-face nil :foreground "olive drab")
(set-face-attribute 'font-lock-doc-face nil :foreground "gray50")
(set-face-attribute 'font-lock-function-name-face nil :foreground "burlywood3")
(set-face-attribute 'font-lock-keyword-face nil :foreground "DarkGoldenrod3")
(set-face-attribute 'font-lock-string-face nil :foreground "olive drab")
(set-face-attribute 'font-lock-type-face nil :foreground "burlywood3")
(set-face-attribute 'font-lock-variable-name-face nil :foreground "burlywood3")

; building a project stuff
(require 'compile)

(setq denis-makescript "build.bat")
(setq compilation-directory-locked nil)

; Compilation
(setq compilation-context-lines 0)
(setq compilation-error-regexp-alist
    (cons '("^\\([0-9]+>\\)?\\(\\(?:[a-zA-Z]:\\)?[^:(\t\n]+\\)(\\([0-9]+\\)) : \\(?:fatal error\\|warnin\\(g\\)\\) C[0-9]+:" 2 3 nil (4))
     compilation-error-regexp-alist))

(defun find-project-directory-recursive ()
  "Recursively search for a makefile."
  (interactive)
  (if (file-exists-p denis-makescript) t
      (cd "../")
      (find-project-directory-recursive)))

(defun lock-compilation-directory ()
  "The compilation process should NOT hunt for a makefile"
  (interactive)
  (setq compilation-directory-locked t)
  (message "Compilation directory is locked."))

(defun unlock-compilation-directory ()
  "The compilation process SHOULD hunt for a makefile"
  (interactive)
  (setq compilation-directory-locked nil)
  (message "Compilation directory is roaming."))

(defun find-project-directory ()
  "Find the project directory."
  (interactive)
  (setq find-project-from-directory default-directory)
  (switch-to-buffer-other-window "*compilation*")
  (if compilation-directory-locked (cd last-compilation-directory)
  (cd find-project-from-directory)
  (find-project-directory-recursive)
  (setq last-compilation-directory default-directory)))

(defun make-without-asking ()
  "Make the current build."
  (interactive)
  (if (find-project-directory) (compile denis-makescript))
  (other-window 1))
(define-key global-map "\em" 'make-without-asking)

(defun maximize-frame ()
  "Maximize the current frame"
  (interactive)
  ;this works for at least Linux
  ; w32-send-sys-command 61488
  (toggle-frame-maximized))

; make background darker
(defun post-load-stuff ()
  (interactive)
  (menu-bar-mode -1)
  (maximize-frame)
  (set-foreground-color "burlywood3")
  (set-background-color "#161616")
  (set-cursor-color "#40FF40")
  )
(add-hook 'window-setup-hook 'post-load-stuff t)

; coloured text tags
(setq fixme-modes '(c++-mode c-mode emacs-lisp-mode java-mode text-mode fundamental-mode))
(make-face 'font-lock-fixme-face)
(make-face 'font-lock-note-face)
(make-face 'font-lock-study-face)
(make-face 'font-lock-important-face)
 (mapc (lambda (mode)
	 (font-lock-add-keywords
	  mode
	  '(("\\<\\(TODO\\)" 1 'font-lock-fixme-face t)
	    ("\\<\\(STUDY\\)" 1 'font-lock-study-face t)
	    ("\\<\\(IMPORTANT\\)" 1 'font-lock-important-face t)
            ("\\<\\(NOTE\\)" 1 'font-lock-note-face t))))
	fixme-modes)
(modify-face 'font-lock-fixme-face "Red" nil nil t nil t nil nil)
(modify-face 'font-lock-note-face "Dark Green" nil nil t nil t nil nil)
(modify-face 'font-lock-study-face "Yellow" nil nil t nil t nil nil)
(modify-face 'font-lock-important-face "Yellow" nil nil t nil t nil nil)

(defun denis-replace-string (FromString ToString)
  "Replace a string without moving point."
  (interactive "sReplace: \nsReplace: %s With: ")
  (save-excursion
    (replace-string FromString ToString)
    ))

(defun denis-replace-in-region (old-word new-word)
  "Perform a replace-string in the current region."
  (interactive "sReplace: \nsReplace %s With: ")
  (save-excursion (save-restrictions
		   (narrow-to-region (mark) (point))
		   (beginning-of-buffer)
		   (replace-string old-word new-word)
		   ))
  )
(defun post-load-stuff ()
  (interactive)
  (menu-bar-mode -1)
  (maximize-frame)
  (set-foreground-color "burlywood3")
  (set-background-color "#161616")
  (set-cursor-color "#40FF40")
)
(add-hook 'window-setup-hook 'post-load-stuff t)

(split-window-horizontally)

(require 'asm-mode)
(add-hook 'asm-mode-hook (lambda ()
			   (electric-indent-mode -1)
			   (setq ess-fancy-comments nil)))

; C/C++ indentation style
(defconst denis-c-style
  '((c-comment-only-line-offset    . 0)
    (c-hanging-braces-alist        . ((class-open)
				      (class-close)
				      (defun-open)
				      (defun-close)
				      (inline-open)
				      (inline close)
				      (brace-list-open)
				      (brace-list-close)
				      (brace-list-intro)
				      (brace-list-entry)
				      (block-open)
				      (block-close)
				      (substatement-open)
				      (statement-case-open)
				      (class-open)))
    (c-hanging-colons-alist        . ((inher-intro)
				      (case-label)
				      (label)
				      (access-label)
				      (access-key)
				      (member-init-intro)))
    (c-cleanup-list                . (scope-operator
				      list-close-comma
				      defun-close-semi))
    (c-offsets-alist               . ((arglist-close            .  c-lineup-arglist)
				      (label                    . -4)
				      (access-label             . -4)
				      (substatement-open        .  0)
				      (statement-case-intro     .  4)
				      (statement-block-intro    .  c-lineup-for)
				      (case-label               .  4)
				      (block-open               .  0)
				      (inline-open              .  0)
				      (topmost-intro-cont       . -4)
				      (knr-argdecl-cont         . -4)
				      (brace-list-open          .  0)
				      (brace-list-intro         .  4)))
    (c-echo-syntactic-information-p . t))
  "Denis's C Style")

(defun denis-c-hook ()
					;set style for current buffer
  (c-add-style "DenisC" denis-c-style t))

(add-hook 'c-mode-common-hook 'denis-c-hook)

(setq scroll-step 3)

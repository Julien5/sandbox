;; -*- lexical-binding: t -*-
;;(setq lexical-binding t)

(message "* project-current()")
(setq pcurrent (project-current t))
(message "* project-current %s" pcurrent)

(message "* project-root()")
(setq proot (project-root pcurrent))
(message "* project-root %s" proot)

;;(project-shell-command)
;;(project-search "(print*")

(message "* hash table")
(setq L1 '((k1 v1) (k2  "v2")))
(setq L2 '("k1" "v1" "k2"  "v2"))

(setq M (make-hash-table :test 'equal))
(puthash "hk1" "hv1" M)
(print (gethash "hk1" M))

(message "* car")
(print (car '("a" "b")))
(print (car '(c d)))

(message "* plist")
(setq L1 '(k1 v1));
(setq L2 (quote (kk1 vv1)))
(print (plist-get L1 'k1))
(print (plist-get L2 'kk1))

(message "* list")
(list 'A 'B)
(print (list 'A 'B))

(message "* quote")
(print (quote (a b)))
;;(print (cdr (assoc 'k1 L1)))


(message "* symbol")
(setq a '(+ 1 1))
(setq b '(a 1 2 (d 2 4)))
(print b)
(print a)
(print (eval a))
;;(print '(+ 1 1))
;;(print (+ 1 1))

(message "done")


(message "%s" 
		 `("John" "Jane" "Chad" ,(current-time-string)))

(message "* project-current %s" (project-root (project-current t)))

(setq goodbye
	  (lambda () (message "%s" "hello lambda"))
  )
(funcall goodbye)

((lambda (a b c) (+ a b c))
 1 (* 2 3) (- 5 4))


(defun foo () (print "bar"))
(foo)

(setf (symbol-function 'foo) (lambda () (print "fun")))
(foo)

(defun create-counter ()
  (let ((c 0))
    (lambda ()
      (setq c (+ c 1))
      c)))

(setq counter (create-counter))

(message "%s" (funcall counter)) 
(message "%s" (funcall counter)) 
(message "%s" (funcall counter))

(defun jbo/length (x)
  (if (not x)
	  0
	(+ (jbo/length (cdr x)) 1)
	)
  )

(message "l=%d" (jbo/length '(a b c d)))

(defun distance (f1 f2)
  (defun common (A B)
	(let ((a (car A))
		  (b (car B))
		  )
	  (if (and (string= a b) (and a b))
		  (+ (common (cdr A) (cdr B)) 1)
		0
		)
	  )
	)
  
  (let ((L1 (split-string (file-name-directory f1) "/"))
		(L2 (split-string (file-name-directory f2) "/"))
		)
	(let ((ret (common L1 L2)))
	  (message "%s %s %d" f1 f2 ret)
	  ret
	  ))
  )

(defun my-other-file ()
  (setq f "embedded/compteur/application.cpp")
  (setq L (projectile-get-other-files f))
  (setq Q nil)
  (dolist (o L)
	(let ((d (distance f o)))
	  (if (not Q)
		  (setq Q (list (cons o d)))
		(setq Q (append (list (cons o d)) Q))
		)
	  )
	)
  (sort Q (lambda (od1 od2) (> (cdr od1) (cdr od2))))
  (message "Q=%s" Q)
  (message "file=%s other=%s" f (car (car Q)))
  f
  )

(my-other-file)



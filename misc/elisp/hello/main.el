(message "ok")
;;(setq shell-command "pwd")

(load "jbo-utils")
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

(message "hello")

(defun print (a)
  (message "%s" a)
  )


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


;;(with-current-buffer (get-buffer "*Messages*") (goto-char (point-max)))


(define tree '((1 2) (3 4)))

(define a (flatten 200 tree))

(check (eq (unflatten a) tree))

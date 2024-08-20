(define r1 (= (str-find "-str-" "STR" 'case-insensitive) 1))

(define r2 (= (str-find "-str-str-" "STR" 0 1 'case-insensitive) 5))

(define r3 (= (str-find "-str-str-" "STR" 2 'case-insensitive) 5))

(define r4 (= (str-find "-str-str-" "STR" 'left 'case-insensitive) 5))

(define r5 (= (str-find "-ab-ab-" "AB" 5 'left 'case-insensitive) 4))

(define r6 (= (str-find "-ab-ba-" '("BA" "AB") 0 1 'case-insensitive) 4))

(define r7 (= (str-find "a--a" "A" -1 'left 'case-insensitive) 3))

(check (and r1 r2 r3 r4 r5 r6 r7))

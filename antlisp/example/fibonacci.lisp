(defun fib_rec (target_n n prev_prev_sum prev_sum)
  (cond
    (
      (< n target_n)
      (fib_rec
        target_n (+ n 1) prev_sum (+ prev_prev_sum prev_sum)
      )
    )
    (true (+ prev_prev_sum prev_sum))
  )
)
(fib_rec 10 0 0 1)

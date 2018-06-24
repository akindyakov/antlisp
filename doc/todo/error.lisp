(defun return_error (msg)
    (error msg)
)

(defun retry (value message code)
  (cond
    ( (is_error value) (error:inherit value message code) )
    ( t value )
  )
)

(defun try_catch (value message code)
  (cond
    ( (is_error value) (error:inherit value message code) )
    ( t value )
  )
)

(defun try_else (value default_value)
  (cond
    ( (is_error value) default_value )
    ( t value )
  )
)

(defun hanle_error ()
  (retry (return_error "message") "df" 0)
  (try_catch (return_error "message") (error "df"))
  (try_else (return_error "message"))
)

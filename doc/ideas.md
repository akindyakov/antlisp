# Ideas

## Lisp RPC

## Lisp in argument parsing tasks

```
./usr/bin/something \
    -S 'https://stackoverflow.com'
    -a health.hope \
    -m resist \
    -s and.bite \
    -e 'extern,first.cfg,first.table:extern,second.cfg,second.table'
    -t 16
    -c
```

```
./usr/bin/something \
    -S 'https://stackoverflow.com'
    -a health.hope \
    -m resist \
    -s bite \
    -e 'extern,first.cfg,first.table'
    -e 'extern,second.cfg,second.table
    -t 16
```

```
./usr/bin/something \
    --source 'https://stackoverflow.com'
    --add health.hope \
    --mode resist \
    --submode bite \
    --extend 'extern,first.cfg,first.table'
    --extend 'extern,second.cfg,second.table
    --threads 16
```

```
./usr/bin/something '(
    (setq source "https://stackoverflow.com")
    (setq add "health.hope")
    (setq mode "resist")
    (setq submode "bite")
    (setq extend "extern,first.cfg,first.table")
    (setq extend "extern,second.cfg,second.table")
    (setq threads 16)
)'
```

```
./usr/bin/something '(search
    {source "https://stackoverflow.com"}
    {add "health.hope"}
    {mode "resist"}
    {submode "bite"}
    {extend
        (extern "first.cfg" "first.table")
        (extern "second.cfg" "second.table")
    }
    {threads 16}
)'
```

## Tables
```
(table:table 4 nil
  (table:content
    (table:row
      (table:cell 1 "")
      (table:cell 1 "")
      (table:cell 1 "")
      (table:cell 1 "")
    )
    (table:row
      (table:cell 1 "")
      (table:cell 1 "")
      (table:cell 1 "")
      (table:cell 1 "")
    )
  )
)
```

# MUSICUI

  - music visualiser in c
  - using raylib
  - hot reloadable
  - currently only macos

## WORK IN PROGRESS

## USAGE

* to build the binary
``` sh
  ./build.sh
```


* to run binary
``` sh
  ./build/musicui <PATH_TO_MUSIC>
```

**when trying to execute binary sometimes the dyldlib path is not set so incase try again after below command**
``` sh
  source .env
``` 

## KEY MAP

  - Key_UP/ KEY_DOWN : volume increase / decrease
  - Key_SPACE : pause music
  - Key_Q : restart the song
  - Key_R : reload the binaries

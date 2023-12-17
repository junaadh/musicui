# MUSICUI

  - music visualiser in c
  - using raylib
  - hot reloadable
  - can toggle release profile
  - release profile createds one static linked exec
  - currently only macos
  - drag and drop to play music

## WORK IN PROGRESS

## USAGE

* to build the binary development
  - hot reloadable
``` sh
  ./build.sh
```

* to build the binary release
  - non hot reloadable
``` sh
  ./build.sh --release
```

* to run application
``` sh
  ./build/musicui
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
  - Key_ESC : quit app

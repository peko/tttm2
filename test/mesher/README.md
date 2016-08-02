Modules
  
  1. main glfw setup

  2. gui -> shape loader
  3. gui -> teseltion controls

  4. shape drawing
  5. teselatiion drawing


Errors in GLFW
--------------

Some window managers cause next error:

```
X Error of failed request:  BadMatch (invalid parameter attributes)
  Major opcode of failed request:  42 (X_SetInputFocus)
  Serial number of failed request:  136
  Current serial number in output stream:  138
```

FIX

XSetInputFocus(...) in lib/glfw/src/x11_window.c
must be commented
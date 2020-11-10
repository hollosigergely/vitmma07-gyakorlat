# UWB távolság mérő szoftver

A firmware alkalmas UWB eszközök között az időbélyegek rögzítésére. 

## Mérés
A mérés során a tageket bekapcsoljuk. Ha bekapcsoláskor (vagy reset esetén) a USER gombot nyomva tartjuk, az eszköz tag módban indul el. A tag automatikusan felderíti a környezetét. A futás során a tag eszköz SEGGER log kimenetét használjuk. Ezt a következő paranccsal érhetjük el:
```
JLinkGDBServer -if SWD -device nRF52
netcat localhost 19021
```

A kimenetre az eszköz vagy naplóbejegyzést ír, ekkor `[` jellel kezdődik a sor, vagy mérési sort ír ki. A mérési sorok formátuma a következő:
```
ssss nn p tttttttttttttt
```
, ahol `s` a forrás eszköz címe, `n` a csomag sorszáma, `p` az időbélyeg típusa, és `t` az időbélyeg. A lehetséges időbélyeg típusok:
* 0: Poll TX
* 1: Poll RX
* 2: Resp TX
* 3: Resp RX
* 4: Final TX
* 5: Final RX

## Eszközök telepítése
Az eszközöket programozhatjuk a megfelelő parancsokkal. A firmware kiírása:
```
nrfjprog  -f  nrf52  --program  firmware.hex  --sectorerase
```

Az eszközök címét a user memóriába írhatjuk:
```
nrfjprog -f nrf52 --eraseuicr
nrfjprog -f nrf52 --memwr 0x10001080 --val 0x<addr>
```

## Feldolgozó szoftver
A feldolgozó szoftver Octave-ban készült, ugyanakkor MATLAB-ban is futtatható. A példa mérésben három eszköz szerepel:
- 0000: tag
- 0001: anchor (a tagtől 530 cm-re)
- 0002: anchor (a tagtől 234 cm-re)

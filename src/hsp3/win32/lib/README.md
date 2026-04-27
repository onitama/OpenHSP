# ライブラリ

## libffi

libffi-3.5.2 では公式のバイナリ配布にはDLL版しか含まれないため、次のコマンドでビルドする。

```bash
../configure CC="../msvcc.sh -m64"  CXX="../msvcc.sh -m64" LD=link CPP="cl -nologo -EP" CXXCPP="cl -nologo -EP" CPPFLAGS="-DFFI_STATIC_BUILD -DUSE_STATIC_RTL"  --enable-static --disable-shared
```
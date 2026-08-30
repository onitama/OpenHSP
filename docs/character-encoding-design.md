# 文字コード設計方針

更新日: 2026-08-30
対象: OpenHSP のランタイム、コンパイラ、Windows DLL、パス・ファイルI/O

この資料は、文字コードを「HSP内部文字列」「パス表現」「外部インターフェース」の3層に分けて整理する。特に、C/C++のビルドマクロ `HSPUTF8` と `HSP_PATHIO_UTF8`、HSPソースに見える `_hsputf8` を混同しないことを目的とする。

## 結論

- HSPランタイムの内部文字列は、`HSPUTF8` が定める。UTF-8ランタイムはUTF-8、旧WindowsランタイムはWindows ACP（日本語環境では通常CP932）である。
- パスI/Oの内部表現は、`HSP_PATHIO_UTF8` の定義有無で定める。定義時はUTF-8、未定義時はWindows ACPである。
- WindowsのOS APIへUnicodeパスを渡すときはUTF-8からUTF-16へ変換して `W` APIを使う。`char*` の互換境界はWindows ACPとして扱う。
- `CP932` は日本語Windowsでの実際のACPになり得るが、公開契約の名前には使わない。固定CP932ではなく `CP_ACP` である。
- パスの文字コードと、HSPソースの文字コード・コンパイル後文字列の文字コードは別の設定である。

## 用語

| 用語 | このリポジトリでの意味 |
| --- | --- |
| HSP内部文字列 | `char` のNUL終端バイト列。HSPの変数、ランタイム内部処理、通常のHSP APIが扱う文字列 |
| UTF-8 | パスおよびUTF-8ランタイムで使う妥当性検査対象のUTF-8バイト列 |
| ACP / legacy | Windowsの現在のANSIコードページ。Win32では `CP_ACP`。POSIXでは特定コードページを意味しない互換用バイト列 |
| UTF-16 | WindowsのワイドAPI (`wchar_t` / `W` API) の境界表現 |
| `path_view` | ビルド時にUTF-8またはACPへ固定される型付きパス入力 |
| source text | HSPコンパイラへ渡す `.hsp` / `.as` のソース本文。パスとは別の文字コード契約を持つ |

## 環境ごとの規約

| 環境・ビルド | `HSPUTF8` | `HSP_PATHIO_UTF8` | HSP内部文字列 | `path_view` /通常パス | Windows OS境界 |
| --- | --- | --- | --- | --- | --- |
| 旧Windowsランタイム | 未定義 | 未定義 | ACP（日本語環境では従来SJIS/CP932） | `ansi_view` / ACP | narrow APIまたはACP変換 |
| Windows UTF-8ランタイム | 定義 | 定義 | UTF-8 | `utf8_view` / UTF-8 | UTF-8 → UTF-16 → `W` API |
| Windows `hspcmp` / `hspcmp.dll` | 未定義 | 定義 | コンパイラのパスはUTF-8。ソース本文は入力モード依存 | `utf8_view` / UTF-8 | `wmain` のUTF-16引数をUTF-8化。DLLの `char*` はACP |
| Linux / macOS / iOS / Android / Emscripten | `hsp3config.h` またはビルド定義で定義 | 定義 | UTF-8 | `utf8_view` / UTF-8 | UTF-8バイト列をPOSIX系APIへ渡す |
| Windows版HSP3Dish等 | プロジェクトで定義 | 定義 | UTF-8 | `utf8_view` / UTF-8 | UTF-8 → UTF-16 → `W` API |

パスIOの文字コードは [`hsp3config.h`](../src/hsp3/hsp3config.h) または各ビルド設定で `HSP_PATHIO_UTF8` を定義して選択する。

`HSP_PATHIO_UTF8` はコンパイラとランタイムのビルド設定で直接定義する。`HSPUTF8` から別のマクロを自動生成しないため、パス表現の選択はこの定義だけで決まる。

## マクロの責務

### `HSPUTF8`（C/C++ビルドマクロ）

HSPランタイムの内部文字列表現を選ぶマクロである。主な影響範囲は次のとおり。

- `src/hsp3/hsp3config.h` で、GCC系ターゲットでは自動定義される。Windows系は各プロジェクトが定義する。
- [`supio.h`](../src/hsp3/supio.h) の `STRLEN` とWindows `supio_win` / `supio_win_unicode` の実装選択を切り替える。
- Windowsでは `HSPAPICHAR` を `wchar_t` とし、`chartoapichar` などでHSP内部UTF-8とUTF-16 API文字列を変換する。
- 未定義時のWindows HSP内部文字列はACPとして扱い、通常の `char*` APIへそのまま接続できる。

これは「Windowsの全ての `char*` がUTF-8」という意味ではない。外部のlegacy `char*` は別にACP境界として残る。

### `HSP_PATHIO_UTF8`（パスI/Oの表現マクロ）

パスI/Oの標準表現を選ぶマクロである。UTF-8パスを使うターゲットで定義し、旧WindowsのACPパスを使うターゲットでは未定義にする。現在の `hspcmp`、`hspcmp.dll`、UTF-8ランタイムのプロジェクトで定義される。

- `hsp_path::path_view`、`hsp_path_fopen`、`hsp_path_filesize`、`getpath` などの通常パスAPIをUTF-8側へ接続する。
- Windowsの `wmain` 引数はUTF-16からUTF-8へ変換して、コンパイラ内部の `std::string` パスにする。
- DLLの外部 `char*` パスを受け取った場合はACPとしてUTF-8へ変換する。

`HSP_PATHIO_UTF8` はHSPランタイムの内部文字列をUTF-8へ変更しない。内部文字列の選択は引き続き `HSPUTF8` が担当する。

`path_view` は `HSP_PATHIO_UTF8` の定義有無から直接選ばれる。個々の呼び出し側で `HSPUTF8` とOS条件を再判定しない。

```cpp
namespace hsp_path {
typedef view<utf8_tag> utf8_view;
typedef view<ansi_tag> ansi_view;
typedef utf8_view path_view;  // HSP_PATHIO_UTF8 の場合
}
```

`utf8_view` と `ansi_view` は同じ `char*` を包むだけだが、型を分けることで変換なしの取り違えをコンパイル時に検出する。`ansi_view` はWindowsではACP、POSIXでは互換バイト列であり、CP932型ではない。

### `_hsputf8`（HSPソース用の定義済みマクロ）

これはC/C++プリプロセッサの `HSPUTF8` とは別物である。コンパイラが入力ソースをUTF-8として処理するとき、`HSC3_OPT_UTF8IN` によりHSPソース側へ `_hsputf8` を登録する。

関連する設定は次のとおり。

| 設定 | 意味 |
| --- | --- |
| `HSC3_OPT_UTF8IN` (`32`) | HSPソース本文をUTF-8入力として解析する |
| `HSC3_OPT_UTF8OUT` (`64`) | コンパイル後の文字列コードをUTF-8にするためのオプション |
| `HSC3_MODE_UTF8` (`4`) | コード生成時のUTF-8出力モード |
| `-i` / `-j` | CLIからUTF-8ソース入力を指定する。`-i` は現実装でUTF-8出力モードも設定する |
| `-u` | CLIからUTF-8文字列出力を指定する |
| `#cmpopt utf8 1` | HSPソースからコンパイル後の文字列をUTF-8出力にする |
| `#bootopt utf8` | ランタイム起動時のUTF-8文字列設定。C/C++マクロとは別の実行時設定 |

ソース本文がSJIS/ACPかUTF-8かは `pp_utf8` と入力オプションで決まり、パス表現マクロでは決まらない。WindowsのUTF-8パスビルドでSJISソースを処理する場合、ファイル名をHSP文字列リテラルへ埋め込む箇所だけUTF-8からACPへ変換する。

## APIの文字コード契約

### パスI/O

| API | 入力契約 | 出力・動作 | 用途 |
| --- | --- | --- | --- |
| `hsp_path_fopen_utf8` | UTF-8 | ファイルを開く | 文字コードが明示的にUTF-8の境界 |
| `hsp_path_fopen` | `path_view` | ビルド標準表現で開く | ランタイム・コンパイラ内部の通常パス |
| `hsp_path_from_ansi` | Windows ACPの `ansi_view` | UTF-8 `std::string` | DLLやlegacy APIから入ったパス |
| `hsp_path_to_ansi` | UTF-8 | Windows ACP `std::string` | legacy `char*` 出力境界 |
| `hsp_path_utf8_from_wide` | UTF-16 | UTF-8 `std::string` | Windows `W` APIからの結果 |
| `hsp_path_get_*_utf8` | UTF-8 | UTF-8 | モジュール、カレント、HSPTV等のOSパス |
| `hsp_path_get_*` | `path_view` | `path_view`相当 | 標準内部表現の互換ラッパー |
| `getpath` | 標準内部表現の `std::string` | 同じ表現の `std::string` | パスの語彙的な分解。ファイルI/Oはしない |

WindowsのUTF-8パスI/Oは、おおむね次の流れになる。

```text
HSP内部またはコンパイラのUTF-8 path
        -> UTF-16
        -> _wfopen / FindFirstFileW / GetModuleFileNameW / CreateProcessW
```

旧Windows標準表現の通常ファイルI/Oは、`path_view == ansi_view` のため `fopen`、`_stat64`、`remove` などのnarrow APIへ接続する。共通のUTF-8 APIを直接呼ぶ場合だけ `hsp_path_from_ansi` / `hsp_path_to_ansi` で境界を越える。

`hsp_pathio` の変換は、NULL、不正UTF-8、ACPへ表現できない文字などを成功扱いにしない。UTF-8からACPへの変換で代替文字が発生する場合も失敗とする。

### Windows HSP API補助クラス

[`hsp3utfcnv.h`](../src/hsp3/hsp3utfcnv.h) の次の関数はHSP内部文字列とWindows API文字列の変換を担当する。

| 関数・型 | `HSPUTF8` 定義時 | 未定義時 |
| --- | --- | --- |
| `HSPAPICHAR` | `wchar_t` | `char` |
| `chartoapichar` | HSP UTF-8 → UTF-16 | そのまま |
| `apichartohspchar` | UTF-16 → HSP UTF-8 | そのまま |
| `ansichartoapichar` | ACP → UTF-16 | そのまま |
| `apichartoansichar` | UTF-16 → ACP | そのまま |
| `hsp3_to_utf16` / `utf16_to_hsp3` | UTF-8との変換 | ACPとの変換 |

一方、`utf8_to_utf16_strict`、`utf16_to_utf8_strict`、`ansi_to_utf16_strict`、`utf16_to_ansi_strict` は `HSPUTF8` に依存しない。明示された境界の変換に使用する。

### コンパイラDLL

`src/hspcmp/win32dll/hspcmp3.cpp` の公開関数は歴史的に `char*` ABIを持つため、Windowsではlegacy ACPを外部契約とする。

```text
DLL char* path (ACP)
        -> hsp_path_from_ansi
        -> compiler std::string path (UTF-8)
        -> path I/O / code generation

compiler UTF-8 path
        -> hsp_path_to_ansi
        -> DLL char* output or message (ACP)
```

`hspcmp_message_path` はUTF-8パスをDLLのエラー・メッセージ出力向けACPへ変換する。パスではないAHT本文や通常メッセージは、パス変換関数へ通さず、それぞれの既存の文字列契約を保つ。

## ソース本文とパスの分離

コンパイラでは、次の2つの変換を分ける。

1. **パス変換**: `hspcmp` の入力パス、include、pack、出力先、モジュールパスをUTF-8で扱う。Windows DLLからの入力だけACPからUTF-8へ変換する。
2. **ソース変換**: `.hsp` / `.as` の本文を入力モードに応じてSJIS/ACPまたはUTF-8へ変換し、HSPのコンパイル後文字列コードを出力モードに応じて決める。

したがって、次の命名・変更を避ける。

- `ConvSJis2Utf8` / `ConvUtf82SJis` をパス変換APIと呼ばない。これはWindowsコンパイラのソース本文変換である。
- `HSPUTF8` を見てDLLの `char*` をUTF-8と推定しない。DLLのlegacy ABIはACPである。
- `HSP_PATHIO_UTF8` を見てHSPランタイムの文字列リテラルの内部形式を変更したと解釈しない。
- `ansi` を固定CP932の別名として新しい型・API名に使わない。

## 実装ルール

今後、文字列またはパスを追加・変更するときは次を守る。

1. 関数コメントで入力と出力の文字コードを明記する。`char*` だけでは契約を表さない。
2. パスは可能な限り `hsp_path::utf8_view`、`hsp_path::ansi_view`、`hsp_path::path_view` で受ける。
3. WindowsのUnicodeパスは `W` APIへ接続し、UTF-8とUTF-16の変換を境界に閉じ込める。
4. 通常パスは `hsp_path_*` を使う。`HSPUTF8` とOS条件を各呼び出し側で複製しない。
5. HSPソース本文のSJIS/UTF-8処理とパスI/Oを同じ分岐にまとめない。
6. 変換不能、入力不正、出力領域不足はエラーにする。切り詰め、代替文字、暗黙の再解釈で続行しない。
7. `std::string` の内容をUTF-8として扱う場合は、境界で妥当性を検査する。HSP文字列はNUL終端のバイト列なので、バイト位置で切る処理にも注意する。

## 検証項目

最小限、次のケースを維持する。

- UTF-8の日本語、4バイト文字を含むファイル名・ディレクトリ名。
- 不正UTF-8、途中で切れたUTF-8、NULを含む入力。
- WindowsのACPで表現可能な名前と、UTF-8にはあるがACPで表現できない名前。
- `utf8_view` と `ansi_view` が同じ型になっていないこと。
- Windows UTF-8ランタイム、旧Windows ACPランタイム、Windows `hspcmp` の各パス分岐。
- UTF-8ソース入力、ACP/SJISソース入力、コンパイル後UTF-8文字列出力。
- DLLのACP `char*` パス、コンパイラ内部UTF-8パス、エラー表示用ACPパス。

対応する既存テストは [`test/test_hsp3pathio.cpp`](../test/test_hsp3pathio.cpp) と [`test/test_unicode_path_handling/`](../test/test_unicode_path_handling/) にある。

## `getpath` の最小契約

- `getpath(source, result, mode)` はファイルシステムへアクセスせず、パスを語彙的に分解する。入力は変更しない。
- 成功時は `true`、結果を `result` に格納する。NULL相当、不正な文字コード、変換失敗は `false` とし、`result` は空にする。
- `mode` は `0`:そのまま、`1`:拡張子を除くファイル名、`2`:拡張子、`8`:ファイル名＋拡張子、`32`:ディレクトリ、`16`:ASCII英字を小文字化する。
- `hsp_path_getpath_utf8` は明示的UTF-8、`hsp_path_getpath` はビルド標準表現 (`path_view`) を入力とする互換アダプターである。
- 固定長バッファへ切り詰めず、出力領域不足は失敗にする。

## 参照実装

- パス表現・変換: [`src/hsp3/hsp3pathio.h`](../src/hsp3/hsp3pathio.h)、[`src/hsp3/hsp3pathio.cpp`](../src/hsp3/hsp3pathio.cpp)
- HSP内部/API文字列変換: [`src/hsp3/hsp3utfcnv.h`](../src/hsp3/hsp3utfcnv.h)、[`src/hsp3/hsp3utfcnv.cpp`](../src/hsp3/hsp3utfcnv.cpp)
- HSPランタイムのマクロ選択: [`src/hsp3/hsp3config.h`](../src/hsp3/hsp3config.h)、[`src/hsp3/supio.h`](../src/hsp3/supio.h)
- コンパイラの入力・出力モード: [`src/hspcmp/hsc3.h`](../src/hspcmp/hsc3.h)、[`src/hspcmp/main.cpp`](../src/hspcmp/main.cpp)、[`src/hspcmp/hsc3.cpp`](../src/hspcmp/hsc3.cpp)
- コンパイラのソース本文変換: [`src/hspcmp/token.cpp`](../src/hspcmp/token.cpp)
- Windows DLL境界: [`src/hspcmp/win32dll/hspcmp3.cpp`](../src/hspcmp/win32dll/hspcmp3.cpp)、[`src/hspcmp/supio.h`](../src/hspcmp/supio.h)

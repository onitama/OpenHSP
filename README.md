------------------------------------------------------------------------------
HSP : Hot Soup Processor  
ホットスーププロセッサ  
copyright 1997-2018 (c) onion software/onitama  
------------------------------------------------------------------------------

# はじめに

このフォルダには、OpenHSP/Hot Soup Processorビルド用のファイルが含まれています。
HSP3の機能やSDKを検証することが可能です。
βなどテスト用ブランチに含まれる内容は、未実装の機能や、不具合が含まれていることをご了承の上お使い下さい。


# 動作環境

LinuxのGUI環境(X Window System)で動作します。
一部の機能は、OpenGL及びSDLライブラリを使用して動作します。


# インストール

githubから最新のリポジトリを取得してご使用ください。

	git clone http://github.com/onitama/OpenHSP

取得したリポジトリのソースをコンパイルしてください。
コンパイルには、gcc及びmakeを実行できる環境が必要です。
コンパイルの際には、以下のライブラリが必要になりますので、あらかじめ確認を行なって下さい。

	OpenGLES2.0以降 / EGL
	SDL1.2
	gtk+-2

アーカイブにはソースのみが含まれていますので、makeによってコンパイルする必要があります。
(Linuxのバージョンやディストリビューションによって正しくコンパイルされない場合は、修正が必要になります。)


	make

アーカイブの内容が展開されたディレクトリでmakeコマンドを実行してください。
必要なツールのコンパイルが行なわれ、HSP3が使用できる状態になります。


# Raspberry Piインストール

Raspberry Pi上のRaspbian上で動作します。(推奨バージョンは、September 2017 Kernel version4.9以降です)
hsp3dish及び、hsed(スクリプトエディタ)は、GUI環境でのみ動作します。
(描画に関する機能は、OpenGL及びSDLライブラリを使用して動作しています。)

githubから最新版を取得してください。

	git clone http://github.com/onitama/OpenHSP

Raspberry Piインストール用にpisetup.shスクリプトを同梱しています。
/home/pi/OpenHSPに最新版を取得されている状態で、

	bash /home/pi/OpenHSP/pisetup.sh

を実行することで、HSPのコンパイル及びメニューへの登録を行います。

自前でmakeコマンドを使用してHSPのツールセットを準備する場合は以下のような流れになります。
コンパイルには、gcc及びmakeを実行できる環境が必要になります。
コンパイルの際には、追加のライブラリが必要になります。ネットワークに
接続されている状態で以下のコマンドを実行することで取得できます。

	sudo apt-get update
	sudo apt-get install libgtk2.0-dev
	sudo apt-get install libglew-dev
	sudo apt-get install libsdl1.2-dev libsdl-image1.2-dev libsdl-mixer1.2-dev libsdl-ttf2.0-dev

makeによってコンパイルを行います。

	make -f makefile.raspbian
		
アーカイブの内容が展開されたディレクトリでmakeコマンドを実行してください。
必要なツールのコンパイルが行なわれ、HSP3が使用できる状態になります。

	./hsed

上記のプログラムを起動することで、スクリプトエディタ(簡易版)がGUIで動作します。
HSP3のスクリプトを記述して、実行することのできる簡易的なエディタです。

Raspberry Pi版では、フルスクリーンで実行を行ないます。
実行の中断は、[ctrl]+[C]か[esc]キーを押してください。
キーボードだ正しく認識されていない場合など、中断ができなくなることがありますので注意してください。
GUIエディタだけでなく、コマンドラインから「./hsp3dish ****.ax」の形で実行を行なうことも可能です。


# 使用方法

HSP3は、オープンソースとして公開されているOpenHSP技術をベースに、
Linux上で手軽にプログラミングを楽しむことができるよう構成されています。
インストールを行なうと、以下のコマンドが生成されます。

	hsed		スクリプトエディタ(簡易版)
	hspcmp		HSP3コードコンパイラ
	hsp3cl		HSP3コマンドラインランタイム
	hsp3dish	HSP3Dishランタイム
	hsp3gp		HGIMG4ランタイム

スクリプトエディタ(簡易版)は、HSP3のスクリプトを記述して、実行することのできる
GUIアプリケーションです。
基本的なスクリプトの編集、及びロード・セーブ機能を持っています。
[F5]キー、または「HSP」メニューから「Run」を選択することで編集している
スクリプトを実行できます。
現在のバージョンでは、標準ランタイムとしてhsp3dishが使用されます。
HSP3Dishに対応するサンプルコードがsampleフォルダに含まれていますので、
お試しください。
スクリプトの文字コードはUTF-8として扱われます。Windowsが使用する文字コード
(SJIS)とは異なりますので注意してください。

コマンドラインからスクリプトの実行を行なう場合は、hspcmpにより
HSPオブジェクトファイルを作成する必要があります。

	./hspcmp -d -i -u test.hsp

上の例では、「test.hsp」ファイルからオブジェクトファイル「test.ax」を生成します。
生成されたオブジェクトファイルを、ランタイムに渡して実行を行ないます。

	./hsp3cl test.ax

上の例では、「test.ax」をHSP3コマンドラインランタイムで実行します。
同様に、「hsp3dish」「hsp3gp」などのランタイムに合わせたスクリプトを
実行させることができます。
(「hsp3dish」「hsp3gp」の実行は、GUI環境が必要になります。)


# exec、devprm命令について

Linux版、Raspberry Pi版ともにexec命令により、シェルのコマンドを呼び出すことができます。
また、devprm命令によりファイルシステム上のデバイスに文字列を出力することが可能です。

	devprm "/sys/class/gpio/export", "2"

のように記述した場合は、「/sys/class/gpio/export」に「2」が出力されます。
シェルから「echo 2 > /sys/class/gpio/export」を実行するのと同じ動作になります。


# Raspberry PiのGPIO入出力

Raspberry Pi版では、devprm命令の他にdevcontrol命令によりGPIO入出力を行なう拡張が行われています。
GPIO出力を制御する場合は、以下のように記述します。

	devcontrol "gpio", ポート番号, 出力値

ポート番号は、GPIOのポートを数値で指定します。
出力値は、1(ON)か0(OFF)を数値で指定することで、デジタルポートの出力を制御します。
入力を行う場合は、以下のように記述します。

	devcontrol "gpioin", ポート番号

命令の実行後にシステム変数statに0か1が代入されます。
(エラーが発生した場合は、マイナス値が代入されます)
GPIO入出力は、hsp3dishだけでなくhsp3clからも使用することが可能です。


# オンラインマニュアル

HSP3.5に関する情報はオンラインマニュアルでご覧いただけます。
http://www.onionsoft.net/hsp/v35/

HSPについての最新情報やコミュニティは、HSPTV!サイトにて提供されています。
http://hsp.tv/


# 将来の予定

HSP3標準ランタイム、及びヘルプリファレンスや周辺ツールなどHSP3で用意されている
機能なども、今後追加される予定です。
まだ不備も多く、必要なものが足りない状況ですが、今後整備したいと考えています。
パッケージの不備やアドバイスなどありましたら、お知らせ頂ければ幸いです。

	onion software (hsp@onionsoft.net)
	http://www.onionsoft.net/

頂いたメールには、すべて目を通しておりますが、返信や、要望の反映などについては
作者がすぐに対応できないこともありますので、予めご了承下さい。
HSPについての一般的な質問や、スクリプトの作り方に関するご質問は、
ネット上のFAQや、書籍などでも情報を提供していますので、まず調べてみることを
お勧め致します。


# 謝辞

HSPスクリプトエディタ(hsed)は、K-K(瓶詰堂)さんのご協力により作成されています。  TTFフォントデータとしてIPAフォントを同梱させて頂きました。
(ライセンスについては、IPAフォントライセンスv1.0を参照してください。)

HSP3及びOpenHSPに多大なご協力を頂いた以下の皆様に感謝致します。  
  
Senchaさん、ゆめゆめゆうかさん、Lonely Wolfさん、Shark++さん、
HyperPageProjectさん、ちょくとさん、S.Programsさん、zakkiさん、
山田 雄己(Yuki)さん、K-Kさん、USKさん、NGND001さん、yoshisさん、
nakaさん、JETさん、ellerさん、さくらさん、うすあじさん、悠黒喧史さん、tomさん、
ぷまさん、arueさん、mjhd_otsukaさん、tds12さん、fujidigさん、naznyarkさん  
  
その他、HSP周辺ツール開発者ML(HSPDev-ML)の皆様及び、β版のテスト報告や、
ご意見をお寄せいただいた多くの方々に感謝致します。  


# ライセンス

HSP3 Linuxは、OpenHSPの派生物として取り扱い、ライセンスもOpenHSP/HSP3に準拠した修正BSDライセンスとなります。  

-------------------------------------------------------------------------------
Hot Soup Processor (HSP) / OpenHSP
Copyright (c) 1997-2018, onion software/onitama
in collaboration with Sencha, Yume-Yume Yuuka, Y-JINN, chobin,
Usuaji, Kenji Yuukoku, puma, tom, sakura, fujidig, zakki, naznyark,
Lonely Wolf, Shark++, HyperPageProject, Chokuto, S.Programs, 
Yuki, K-K, USK, NGND001, yoshis, naka, JET, eller, arue, mjhd_otsuka, tds12
All rights reserved.

These softwares are provided by the copyright holders and contributors "as is" and
any express or implied warranties, including, but not limited to, the implied
warranties of merchantability and fitness for a particular purpose are disclaimed.
-------------------------------------------------------------------------------
                                                HSP users manual / end of file 
-------------------------------------------------------------------------------

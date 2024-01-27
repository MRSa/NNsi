# OSDN NNsi : NNsiのよくある質問と答え

**本ページは、[OSDN の NNsi プロジェクト NNsiのよくある質問と答え](http://osdn.net/projects/nnsi/nnsi-faq.htm) の復元です。**

-----

<!DOCTYPE HTML PUBLIC "-//IBM//DTD HPB HTML//EN">
<HTML>
<HEAD>
<META name="GENERATOR" content="IBM HomePage Builder V3.0.7  for Windows">
<META name="Content-Language" content="ja">
<TITLE>FAQ of NNsi</TITLE>
</HEAD>

<BODY bgcolor="#efefef">

<H1>NNsiのよくある質問と答え</H1>
<P>
NNsiをリリースしてから、質問があった項目をまとめてみました。
</P>
<HR align="right">
<DL>
  <DT><B>NNsiをアップグレードして最初に起動したとき、Fatal Errorが発生し、Palmデバイスがリセットする</B>
  <DD>NNsi設定のデータ構造が変更になった場合、環境によってはFatal Errorが発生することがあります。復旧するためには、Palmデバイス内にあるデータベースファイル<U>Settings-NNsiを削除してください。</U>
  <DT><B>「参照専用ログ」で一覧ボタンを押しても、スレのタイトルが表示されない、もしくはスレタイトルが表示されるがスレデータを参照できない</B>
  <DD>NNsi設定で、「VFSの使用」および「OFFLINEスレをVFSに置く」にチェックが入っていない可能性があります。チェックを入れてください。
  <DT><B>昨日までスレが取得できたのに、今日から急に取得できなくなった</B>
  <DD>板のサーバ移転が実施された可能性があります。「管理」メニューの「BBS一覧取得」を実施して板があるURLを更新してください。（BBS一覧取得先は、NNsi設定-2で設定が可能です。）
  <DT><B>スレ参照中に文字列の選択ができない</B>
  <DD>CLIEのハイレゾ表示モード中には、文字列の選択ができません。「その他」→「描画モード変更(CLIE)」で描画モードを通常表示モードに変更してから文字列の選択を行ってください。
  <DT><B>NNsiから起動に対応しているWebブラウザは？</B>
  <DD>NNsiの編集メニューにある「Webブラウザで開く(選択文字列をブラウザで)」機能に対応しているブラウザは、NetFront,
PalmScape, Xiino, Web Browser 2.0, Blazer です。(PocketLinkには、他のアプリからURLを指定されて開く機能がないようなので対応できませんでした。ごめんなさい。)
  <DT><B>CLIE NXで、書き込みを行おうとすると漢字変換ができない</B>
  <DD>ATOK使用中のCLIE NXで、スレに書き込みを行おうとした場合に、漢字変換できないトラブルが発生します。回避策として
NNsi設定-3に「書き込み時フォーカス移動しない」という設定を用意していますので、これにチェックを入れてみてください。
  <DT><B>スレ参照画面で、スレメッセージを選択することができない</B>
  <DD>グラフィック描画モードになっていることが考えられます。メニューの「その他」→「描画モード変更」でフィールド表示モードに切り替えて再度お試しください。
  <DT><B>「NNsi終了時VFSにDBを保存」または「OFFLINEスレをVFSに置く」チェックを消したはずなのにNNsiを再起動するとチェックが入っている</B>
  <DD>現状は、NNsi設定の「VFSが使用できる時はVFS ON」にチェックが入っており、VFSが使用できる状態だとNNsiが検知すれば、自動的にこれらのチェックを付加する仕様としています。もし、自動的にチェックを入れたくなければ、「VFSが使用できる時はVFS
ON」のチェックをはずしてご使用ください。
  <DT><B>「しおり機能」とは？</B>
  <DD>NNsi起動時、一番最初に開くスレとレス番号を指定する機能です。（<U>もし、指定したスレが存在しない場合には、何もメッセージが表示されていないスレ参照画面が表示されます。</U>） 
スレ参照画面に表示される、&quot;旗&quot; or &quot;M&quot; ボタンを押すとしおりが設定され、NNsi設定-4の「しおり設定」のチェックを外すとしおりの設定が解除されます。
  <DT><B>「ギコのしっぽ」を使って参照専用ログをインストールしたはずなのに読むことができない</B>
  <DD>ギコのしっぽとともにNNsiを利用するには、いくつか気をつける点があります。ご注意ください。
  <OL>
    <LI>『NNsi用に変換(指定フォルダ)』でファイルを一旦指定フォルダに作成しておく 
（直接インストールはうまくいかないようです）
    <LI>HotSyncには、PalmDesktop v4.0 以上を使用する
    <LI>NNsiの右上タブを「参照専用ログ」にあわせ、『一覧』ボタンを押す
  </OL>
  <DT><B>いま参照しているメッセージにURLが書かれている２ちゃんねるスレを取得したい</B>
  <DD>メニュー内にある、『スレ番指定取得』機能がそれにあたります。 たとえば、http://pc3.2ch.net/test/read.cgi/mobile/1037882954/
といったＵＲＬを持つ、２ちゃんねるのスレを取得したい場合には、スレ番指定取得機能を使い、&quot;mobile/1037882954&quot;を入力してスレを取得してください。（なお、このスレ番指定取得機能は、２ちゃんねるのスレのみサポートする機能です。）
</DL>
<HR>
<P align="right">
(Feb/11/2002)
</P>
</BODY>
</HTML>
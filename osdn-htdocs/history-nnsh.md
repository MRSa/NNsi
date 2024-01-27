# OSDN NNsi : NNsi 更新履歴

**本ページは、[OSDN の NNsi プロジェクト NNsi 更新履歴](http://osdn.net/projects/nnsi/history-nnsh.htm) の復元です。**

-----

<P>
<B>NNsi 更新履歴</B><BR>
</P>
<UL>
  <LI><A href="http://downloads.sourceforge.jp/nnsi/2335/NNsi101r000.zip">Version1.01
Revision000</A> : <A href="http://downloads.sourceforge.jp/nnsi/2336/NNsi-src101r000.ZIP">11/Feb/2003</A>
  <UL>
    <LI>PalmOS v3.1機を使用中に、スレ参照画面でスレレベルを変更すると、画面が真っ白になる不具合を修正する。
    <LI>ソート時無効になっているロジックの不具合を修正する。
    <LI>メッセージ末尾の空白のみの行を削るロジックを追加する(NNsi.prcのみ)
    <LI>メッセージ表示中に不要なスクロールが可能になってしまう不具合を修正する
    <LI>スレ一覧画面で、前ページ/次ページに移動するボタンを下部にあつめていたものを右上/右下に配置するよう変更する
    <LI>書き込み画面表示中にアプリを切り替えたとき、編集中メッセージを保存できるように修正する
    <LI>ワークバッファサイズを大きくしたとき、書き込みができなくなっていた不具合を修正する
    <LI>書き込み画面用のバッファサイズをNNsi設定-2で設定できるようにする
    <LI>スレ取得保留機能の実装。(一覧画面の「差分取得」「全取得(再取得)」、参照画面での「差分取得」の３つで有効な機能です。)
    <LI>NNsi設定-7を他のNNsi設定-xに移動、微妙に整理する
    <LI>新着確認中のホスト接続中に現在取得件数と全件数を表示するようにする
    <LI>スレデータ再取得を実施したとき、一時領域を確保するタイミングをスレデータを消去する前に実施するよう変更する
  </UL>
</UL>
<HR>
<UL>
  <LI><A href="http://nnsh.hp.infoseek.co.jp/files/NNsi/NNsi100r001.ZIP">Version
1.00 Revision001</A> : <A href="http://nnsh.hp.infoseek.co.jp/files/NNsi/NNsi-src100r001.ZIP">03/Feb/2003</A>
  <UL>
    <LI>メッセージ参照時に選択メニューで回線切断したとき表示が消える不具合を修正
    <LI>メッセージ参照時に選択メニューでメニューを選択したとき、表示が消える不具合を修正
    <LI>ネットワーク設定パネルを表示できるようにした(一覧画面および参照画面で表示可能です。)
    <LI>スレ参照時、番号フィールドにカーソルを移動させない設定を可能にした。
    <UL>
      <LI>NNsi設定-5の「参照時フォーカス移動しない」です。これを設定すると、参照時のカーソル点滅がなくなります。
    </UL>
    <LI>スレ一覧画面の左上にあるチェックボックスについて、これを「多目的スイッチ１」として、設定したい項目を変更できるようにした。
    <LI>スレ一覧画面の左上(多目的スイッチ１の右隣)に 「多目的スイッチ２」を追加した。
    <LI>多目的スイッチの機能割り当てをNNsi設定-6に追加する
    <LI>選択メニューにいくつかのアイテムを追加できるようにした
    <UL>
      <LI>描画モード変更(参照/一覧)、スレ情報(参照)、多目的SW1/2(一覧)
      <LI>分割取得モード切替チェックボックスを多目的スイッチ化したため、一覧の選択メニューの&quot;取得モード&quot;は廃止しました。
    </UL>
    <LI>NNsi設定-5～NNsi設定-7の設定項目を(軽く)整理した
    <LI>新着確認時、ログ保管場所をチェックせず差分取得してしまう不具合を修正
    <LI>スレ一覧の整列機能を実装（モバ板元７３２さんに頂きました！ありがとうございます！）
    <UL>
      <LI>一覧の「スレ」メニュー内にある、「スレ整列」です。
    </UL>
    <LI>お気に入り設定をレベルわけ(5段階)できるようにしました。（これまたモバ板元７３２さんに頂きました！ありがとうございます！）
    <UL>
      <LI>「スレレベル」という概念を導入しました。「スレ整列」を実行すると、スレレベルが高い(H)ほど、上位にくるようになります。
    </UL>
    <LI>NNsi設定-6に、「お気に入り」に表示する最低レベルを設定できるようにする
    <LI>「スレ情報」画面で、スレレベルの設定を可能にする
    <LI>スレ一覧にあった今までの「お気に入り設定」チェックボックスのところにスレレベルを表示するようにした
    <UL>
      <LI>スレレベル部分をタップすると、スレ情報画面を開きます。
    </UL>
    <LI>「スレ情報」画面の表示内容を整理した
    <UL>
      <LI>今まで、一覧画面/参照画面で違う内容を表示していたが、同じ内容を表示するように変更する
    </UL>
    <LI>板を移動したときに、自動的に未取得一覧を削除する設定項目をNNsi設定-6に追加
    <LI>板一覧を取得したとき、URLで板を判別しないようにする設定項目をNNsi設定-6に追加
    <UL>
      <LI>とりあえず、今後のことを考え追加してみました。（ただし、動作保証はいたしません。あしからず。）
    </UL>
    <LI>「使用BBS選択」画面で、タイトルの検索機能を追加
    <UL>
      <LI>メニューの「タイトル検索」です。
    </UL>
    <LI>「使用BBS選択」画面で、板を登録する機能を追加
    <UL>
      <LI>メニューの「BBS登録」ですが、<U>動作を保証するわけではありません。</U>(今後の掲示板サポート確認作業のために、とりあえず追加した感じです)
    </UL>
    <LI>まちＢＢＳでのメッセージ表示がおかしかった不具合に対処
    <LI>「未読あり」タブ表示中の新着確認で、実施する板カテゴリを設定できるように拡張する
    <UL>
      <LI>これまた、モバ板元７３２さんにいただきました。ありがとうございました。(NNsi設定-6で設定が可能です。)
    </UL>
    <LI>スレ書き込み画面でメッセージ保存したとき、名前およびE-Mail欄が保管されない不具合を修正
  </UL>
</UL>
<HR>
<UL>
  <LI>Version 0.99 Revision002 : <A href="http://nnsh.hp.infoseek.co.jp/files/NNsi/NNsi-src099r002.ZIP">26/Jan/2003</A>
  <UL>
    <LI>ARMletでの半角カナ→全角カナ変換の方法に問題があったため、さらに変更
    <LI>ソースコードのディレクトリ構成を変更。Includeファイル系をIncディレクトリに移動させ、Makefile群を修正
    <LI>メッセージ参照時に選択メニューで回線切断したとき、表示が消える不具合を修正
    <LI>OS31NNsi.prcで、高解像度サポートを外し、バイナリサイズを縮小
  </UL>
  <LI><A href="http://nnsh.hp.infoseek.co.jp/files/NNsi/NNsi-src099r001.ZIP">Version
0.99 Revision001</A> : <A href="http://nnsh.hp.infoseek.co.jp/files/NNsi/NNsi099r001.ZIP">25/Jan/2003</A>
  <UL>
    <LI>PalmOS 3.1機で起動させることができなかった不具合に対処
    <LI>半角カナ→全角カナ変換の方法を変更
    <LI>PalmOS 3.1機で、NNsi設定-6に「色の指定」ラベルが残る不具合に対処
    <LI>スレ一覧画面で、スレの末尾にジャンプしても１ページ目の末尾にしか カーソルが移動しない不具合に対処
  </UL>
  <LI><A href="http://nnsh.hp.infoseek.co.jp/files/NNsi/NNsi-src099r000.ZIP">Version
0.99 Revision000</A> : <A href="http://nnsh.hp.infoseek.co.jp/files/NNsi/NNsi099r000.ZIP">23/Jan/2003</A>
  <UL>
    <LI>スレタイ一覧画面のカラー対応（NNsi設定-6で指定してください。)
    <LI>「書き込みシーケンスその２」で待ちを入れてみた(この設定値は、なるべくＯＦＦでご使用願います。)
    <LI>選択メニューのリストダブルタップでOKボタンを押したのと同等にした
    <LI>OFFLINEスレ検索時に前のスレ情報を削除しなくできるようにした（NNsi設定-6で設定してください。）
    <LI>NNsi設定-3の「参照専用COPY時に削除する」をNNsi設定-6に移動させた
    <LI>NNsi設定-3に「書き込み時アンダーラインOFF」を追加した
    <LI>スレタイ一覧画面の前ページ/次ページのボタン制御(表示および消去)を行うようにした
    <LI>参照時選択メニューで、レス番参照(&gt;&gt;???)の番号が異常だった場合、選択メニューにその番号を表示しないようにした
    <LI>通信時の進捗表示を日本語化した
    <LI>書き込みエラー要因表示ダイアログの表示フォーマットを微妙に変更した
    <LI>書き込みエラー発生(ホストがERROR応答)した時には、書き込み画面から抜けないようにした
    <LI>スレ参照画面の選択メニューで、「１つ戻る」を表示しない設定も可能にした
  </UL>
</UL>
<HR>
<UL>
  <LI><A href="http://nnsh.hp.infoseek.co.jp/files/NNsi/NNsi098r000.ZIP">Version
0.98 Revision000</A> : <A href="http://nnsh.hp.infoseek.co.jp/files/NNsi/NNsi-src098r000.ZIP">14/Jan/2003</A>
  <UL>
    <LI>２ちゃんねるの書き込み仕様変更に暫定対応パート２、NNsi設定-3の「書き込みシーケンスその２」がOFFでも書き込めます。<U><B>(この設定値は、なるべくＯＦＦでご使用願います。)</B></U>
    <LI>書き込み失敗時のエラー要因表示を強化した(つもり)
    <LI>NNsi新規インストール時にBBS一覧を取得するとエラーが発生し、先に進めなかった不具合に対処
    <LI>スレ一覧画面で、スレタイトルを書き換えする機能を追加(メニューに「スレタイトル変更」を追加しました。)
    <LI>スレ参照画面でスレ番指定取得がうまくできていなかった不具合に対処。
  </UL>
</UL>
<HR>
<UL>
  <LI><A href="http://nnsh.hp.infoseek.co.jp/files/NNsi/NNsi097r000.ZIP">Version
0.97 Revision000</A> : <A href="http://nnsh.hp.infoseek.co.jp/files/NNsi/NNsi-src097r000.ZIP">13/Jan/2003</A>
  <UL>
    <LI><B>OS5ハイレゾ(HighDensity)対応 （281さん、本当にありがとうございます！！）</B>
    <LI><B>NNsi設定-3の『書き込みシーケンスその２』を実装 （２ちゃんねるの仕様変更に暫定対応）</B>
    <LI>半角カナ→全角カナ変換機能を追加(NNsi設定-4)
    <LI>NNsi新規インストール時に画面が真っ白になる不具合に対処
    <LI>OS31NNsi.prcのスレ一覧画面で、一覧上下するボタンを他のバイナリと同じ場所になるよう変更
    <LI>Jogボタンで表示されるメニューに設定できる選択肢をより詳細化(NNsi設定-4/NNsi設定-5で指定可能に)
    <LI>スレ番指定取得機能の不具合修正と、スレ参照画面でも使用可能にする。(メニュー/Jキー押下で使用可能です)
    <LI>「新着確認」時に、確認の残り件数（概算値）を表示するように変更する
    <LI>ファイルデータAppend時に不用意なFileSeek()があったのを削除
    <LI>DB参照専用アクセスの関数を用意
    <LI>BUSYウィンドウに表示する文字列の最大長を確認するように変更
  </UL>
</UL>
<HR>
<UL>
  <LI><A href="http://nnsh.hp.infoseek.co.jp/files/NNsi/NNsi096r001.ZIP">Version
0.96 Revision001</A> : <A href="http://nnsh.hp.infoseek.co.jp/files/NNsi/NNsi-src096r001.ZIP">09/Jan/2003</A>
  <UL>
    <LI>スタックサイズを8kBに拡張
    <LI>描画モード変更のショートカット(メニューコマンド)を G に統一
    <LI>Jogで表示されるメニューに表示するアイテムを拡張
    <LI>NNsi設定-5を追加、一部NNsi設定-4から移動させる
    <LI>スレ参照画面と一覧画面の表示モードを独立して設定できるようにした
    <LI>グラフィック描画モードでの無駄な描画を削減
  </UL>
  <LI><A href="http://nnsh.hp.infoseek.co.jp/files/NNsi/NNsi-src096r000.ZIP">Version
0.96 Revision000</A> : <A href="http://nnsh.hp.infoseek.co.jp/files/NNsi/NNsi096r000.ZIP">06/Jan/2003</A>
  <UL>
    <LI><B>Tungsten Tの5way対応コードのマージ(281さん、ありがとうございます！！）</B>
    <LI>スレタイトル一覧画面の不具合修正(まだ修正途中...)
    <UL>
      <LI>ジョグ回しがPowerJOGとブッキングしていた件
      <LI>シルク開閉したときにタイトル一覧が消える件(でもまだ完璧でない...)
      <LI>「新着確認」終了時に、必ず回線が切断されていた件
      <LI>NNsi設定-2のショートカットと描画モード変更のショートカットがブッキングしていた件
    </UL>
    <LI>NNsi終了時にDBをバックアップする設定のとき、<B>暫定的に</B>BUSYウィンドウを出すのをやめる(CLIEのPalmSimでエラーが出るため...)
    <LI>バックボタンで出てくる選択メニューの大きさを拡大
    <LI>参照/一覧画面のとき、従来Jog Backボタンに割り当てていたメニューを表示するオプションをJog
Pushボタンに割り当て可能に(NNsi設定-4)
  </UL>
</UL>
<HR>
<UL>
  <LI><A href="http://nnsh.hp.infoseek.co.jp/files/NNsi/NNsi-src095r000.ZIP">Version
0.95 Revision000</A> : <A href="http://nnsh.hp.infoseek.co.jp/files/NNsi/NNsi095r000.ZIP">05/Jan/2003</A>
  <UL>
    <LI><B>スレタイトル一覧画面の全面変更</B>
    <UL>
      <LI>CLIEシリーズ用にハイレゾ化
      <LI>一覧の生成および表示方法を変更し、応答時間を短縮
      <LI>一覧生成の方法変更に伴い、「スレタイ検索」と「新着確認」のロジックを全面書き直し
    </UL>
    <LI>NNsi設定-5～NNsi設定-7の画面追加(純粋に画面を追加しただけ、まだ何も機能なし)
  </UL>
</UL>
<HR>
<UL>
  <LI><A href="http://nnsh.hp.infoseek.co.jp/files/NNsi/NNsi094r004.ZIP">Version
0.94 Revision004</A> : <A href="http://nnsh.hp.infoseek.co.jp/files/NNsi/NNsi-src094r004.ZIP">11/Jan/2003</A>
  <UL>
    <LI>Tungsten 5way Navigator/Trg用Jogに対応。(281さん、ありがとうございます！)
  </UL>
  <LI><A href="http://nnsh.hp.infoseek.co.jp/files/NNsi/NNsi-src094r003.ZIP">Version
0.94 Revision003</A> : <A href="http://nnsh.hp.infoseek.co.jp/files/NNsi/NNsi094r003.ZIP">29/Dec/2002</A>
  <UL>
    <LI>メッセージ内に入っている異常なNULL文字を無視するよう変更
    <LI>レス数が1005を超えているスレが取得できても参照できなかった不具合に対処
  </UL>
  <LI><A href="http://nnsh.hp.infoseek.co.jp/files/NNsi/NNsi-src094r002.ZIP">Version
0.94 Revision002</A> : <A href="http://nnsh.hp.infoseek.co.jp/files/NNsi/NNsi094r002.ZIP">25/Dec/2002</A>
  <UL>
    <LI>PalmOS4.0以降のデバイスでは、書き込み時に2chへ送信する時刻をUTC基準で計算するように変更する。(これでPalm本体の時刻がPHTでも大丈夫な、はず。)
    <LI>スレ参照画面でしおり設定ボタンをタイトルの左隣に移動し、メッセージ移動矢印の間隔を広げる。
    <LI>新着メッセージ確認時に、ストップチェックを毎回行うようにした。
    <LI>スレ番指定取得で、選択していないBBSを有効にするとBBSリストがおかしくなる不具合に対処したつもり。
  </UL>
  <LI><A href="http://nnsh.hp.infoseek.co.jp/files/NNsi/NNsi-src094r001.ZIP">Version
0.94 Revision001</A> : <A href="http://nnsh.hp.infoseek.co.jp/files/NNsi/NNsi094r001.ZIP">20/Dec/2002</A>
  <UL>
    <LI>サポートWebブラウザにEudora Webを追加する
    <LI>OS31NNsiで、スレ参照画面を開こうとすると無限ループに陥っていた不具合に対処
    <LI>CLIE NX/NR用のシルク制御を行わないNNsi設定-4を追加
    <LI>スレ一覧画面でBackボタンを押すと機能選択メニューが出るようになるNNsi設定-4を追加
  </UL>
</UL>
<HR>
<P>
<A href="history-nnsi2002.md">[これより前の更新履歴]</A>
</P>
<P>
<BR>
<BR>
<BR>
<BR>
<BR>
</P>
<HR>
<P align="right">
(12/Feb/2003)
</P>

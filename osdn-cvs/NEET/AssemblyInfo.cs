using System.Reflection;
using System.Runtime.CompilerServices;

//
// アセンブリに関する一般情報は，次の一連の属性を介して制御されます。 
// アセンブリに関連する情報を変更するには，これらの属性を変更します。
// 
//
[assembly: AssemblyTitle("")]
[assembly: AssemblyDescription("")]
[assembly: AssemblyConfiguration("")]
[assembly: AssemblyCompany("")]
[assembly: AssemblyProduct("")]
[assembly: AssemblyCopyright("")]
[assembly: AssemblyTrademark("")]
[assembly: AssemblyCulture("")]		

//
// アセンブリのバージョン情報は，次の 4 つの値で構成されます。
//
//      メジャーバージョン
//      マイナーバージョン 
//      ビルド番号
//      リビジョン
//
// これらすべての値を指定するか，リビジョンとビルド番号にはデフォルト値を 
// 指定できます。その場合は，次のように '*' を使用します。

[assembly: AssemblyVersion("1.0.*")]

//
// アセンブリに署名するには，使用するキーを指定する必要があります。アセンブリの 
// 署名の詳細については，Microsoft .NET Framework のマニュアルを参照してください。
//
// 署名に使用するキーを制御するには，次の属性を使用します。 
//
// メモ： 
//   (*) キーが指定されなかった場合，アセンブリは署名されません。
//   (*) KeyName は，使用しているコンピュータの Crypto Service Provider
//       （CSP）にインストールされているキーを表します。KeyFile は，キーが格納されている
//       ファイルを表します。
//   (*) KeyFile と KeyName の両方の値が指定された場合は， 
//       次の処理が行われます。
//       (1) その KeyName が CSP で見つかった場合は，そのキーが使用されます。
//       (2) その KeyName が存在せず，その KeyFile が存在する場合は，KeyFile 内の 
//           キーが CSP にインストールされて使用されます。
//   (*) KeyFile を作成するには，sn.exe（Strong Name）ユーティリティを使用します。
//       KeyFile を指定する場合は，KeyFile の場所として，プロジェクトの出力
//       ディレクトリ（%Project Directory%\bin\<configuration>）からの
//       相対パスを使用する必要があります。たとえば，KeyFile がプロジェクト
//       ディレクトリにある場合は，AssemblyKeyFile 属性を次のように指定します。 
//       [assembly: AssemblyKeyFile("..\\..\\mykey.snk")]
//   (*) 遅延署名は高度なオプションです。詳細については，Microsoft .NET Framework
//       のマニュアルを参照してください。
//
[assembly: AssemblyDelaySign(false)]
[assembly: AssemblyKeyFile("")]
[assembly: AssemblyKeyName("")]

using System.Reflection;
using System.Runtime.CompilerServices;

//
// �A�Z���u���Ɋւ����ʏ��́C���̈�A�̑�������Đ��䂳��܂��B 
// �A�Z���u���Ɋ֘A�������ύX����ɂ́C�����̑�����ύX���܂��B
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
// �A�Z���u���̃o�[�W�������́C���� 4 �̒l�ō\������܂��B
//
//      ���W���[�o�[�W����
//      �}�C�i�[�o�[�W���� 
//      �r���h�ԍ�
//      ���r�W����
//
// ����炷�ׂĂ̒l���w�肷�邩�C���r�W�����ƃr���h�ԍ��ɂ̓f�t�H���g�l�� 
// �w��ł��܂��B���̏ꍇ�́C���̂悤�� '*' ���g�p���܂��B

[assembly: AssemblyVersion("1.0.*")]

//
// �A�Z���u���ɏ�������ɂ́C�g�p����L�[���w�肷��K�v������܂��B�A�Z���u���� 
// �����̏ڍׂɂ��ẮCMicrosoft .NET Framework �̃}�j���A�����Q�Ƃ��Ă��������B
//
// �����Ɏg�p����L�[�𐧌䂷��ɂ́C���̑������g�p���܂��B 
//
// �����F 
//   (*) �L�[���w�肳��Ȃ������ꍇ�C�A�Z���u���͏�������܂���B
//   (*) KeyName �́C�g�p���Ă���R���s���[�^�� Crypto Service Provider
//       �iCSP�j�ɃC���X�g�[������Ă���L�[��\���܂��BKeyFile �́C�L�[���i�[����Ă���
//       �t�@�C����\���܂��B
//   (*) KeyFile �� KeyName �̗����̒l���w�肳�ꂽ�ꍇ�́C 
//       ���̏������s���܂��B
//       (1) ���� KeyName �� CSP �Ō��������ꍇ�́C���̃L�[���g�p����܂��B
//       (2) ���� KeyName �����݂����C���� KeyFile �����݂���ꍇ�́CKeyFile ���� 
//           �L�[�� CSP �ɃC���X�g�[������Ďg�p����܂��B
//   (*) KeyFile ���쐬����ɂ́Csn.exe�iStrong Name�j���[�e�B���e�B���g�p���܂��B
//       KeyFile ���w�肷��ꍇ�́CKeyFile �̏ꏊ�Ƃ��āC�v���W�F�N�g�̏o��
//       �f�B���N�g���i%Project Directory%\bin\<configuration>�j�����
//       ���΃p�X���g�p����K�v������܂��B���Ƃ��΁CKeyFile ���v���W�F�N�g
//       �f�B���N�g���ɂ���ꍇ�́CAssemblyKeyFile ���������̂悤�Ɏw�肵�܂��B 
//       [assembly: AssemblyKeyFile("..\\..\\mykey.snk")]
//   (*) �x�������͍��x�ȃI�v�V�����ł��B�ڍׂɂ��ẮCMicrosoft .NET Framework
//       �̃}�j���A�����Q�Ƃ��Ă��������B
//
[assembly: AssemblyDelaySign(false)]
[assembly: AssemblyKeyFile("")]
[assembly: AssemblyKeyName("")]

/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you wish to add, delete or rename functions or slots use
** Qt Designer which will update this file, preserving your code. Create an
** init() function in place of a constructor, and a destroy() function in
** place of a destructor.
*****************************************************************************/

void MainApp::initializeSelf()
{    
    QString userAgent = "Monazilla/1.00 (rootj 0.00)";
    mpHttpAccessor = new rootj::HttpComm(userAgent, barBusy);
    
    mSetting.setPath("nnsi.sourceforge.jp", "qNNsi");
    
    mWidgetMap.clear();
    
    mainTabWidget->setTabEnabled(BoardList, false);
    mainTabWidget->setTabEnabled(ThreadList, false);

    mBoardParser.initializeSelf (barBusy);
    mThreadParser.initializeSelf(barBusy);
    
    QString basePath = mSetting.readEntry("basePath");
    if (basePath.isEmpty() == true)
    {
        basePath = "C:/temp/rootj/";
    }
    edtLogPath->setText(basePath);
    
    QString bbsListUrl = mSetting.readEntry("referenceBbsUrl");
    if (bbsListUrl.isEmpty() == true)
    {
        bbsListUrl = "http://www.ff.iij4u.or.jp/~ch2/bbsmenu.html";
    }
    edtBbsListUrl->setText(bbsListUrl);
    
    // connects the signal/slot (waits for finish...)
    connect(mpHttpAccessor, SIGNAL(signal_finishedHttpCommunication(int)),
            this,           SLOT(slot_finishedHttpCommunication(int)));

    // int pageIndex = mSetting.readEntry("currentPageIndex");
	
    //
	connect(threadList, SIGNAL(doubleClicked(init,int, const QPoint &)),
			this,       SLOT(slot_selectedThread(int,int,const QPoint &)));
	
}

void MainApp::finalizeSelf()
{
    mSetting.writeEntry("basePath", edtLogPath->text());
    mSetting.writeEntry("referenceBbsUrl", edtBbsListUrl->text());
    mSetting.writeEntry("currentPageIndex", mainTabWidget->currentPageIndex());
    
    if (mpHttpAccessor != 0)
    {
        // disconnects the signal/slot (waits for finish...)
        disconnect(mpHttpAccessor, SIGNAL(signal_finishedHttpCommunication(int)),
                   this,           SLOT(slot_finishedHttpCommunication(int)));

        delete mpHttpAccessor;
    }
}

void MainApp::slot_selectedThread(int aRow, int aCol, const QPoint &mousePos)
{
    QMessageBox::information(NULL,QObject::tr("information"),QObject::tr("double clicked."),QMessageBox::Ok);


}

void MainApp::selectLogPath()
{
    QString s = QFileDialog::getExistingDirectory(
                    edtLogPath->text(),
                    this,
                    QObject::tr("get LOG PATH"),
                    QObject::tr("Choose a log directory"),
                    TRUE );
    if (s.isEmpty() != true)
    {
        s = s + "/";
        edtLogPath->setText(s);
    }
}

// to perform action at a "update" button pushed.
void MainApp::update()
{
    // check open page
    int openPage = mainTabWidget->currentPageIndex();
    switch (openPage)
    {
      case 0:
        // 
        updateSettings();
        getBBSlistRequest();
        break;
    
      default:
        break;
    }
}

// updates settings
void MainApp::updateSettings()
{
    // not yet...
}

// updates bbs list
void MainApp::update_BBSlist()
{
    ///// 
    if (mpHttpAccessor == 0)
    {
        return;
    }

    //QMessageBox::information(NULL,QObject::tr("information"),QObject::tr("received BBS list"),QMessageBox::Ok);

    // enables
    mainTabWidget->setTabEnabled(BoardList, true);
    mainTabWidget->setCurrentPage(1);

    lstBoardList->clear();

    lstBoardList->setRootIsDecorated(true);
    //lstBoardList->setSorting(-1,true);
    lstBoardList->setResizeMode(QListView::AllColumns);
    lstBoardList->setColumnText(0, QObject::tr("BBS list"));
    lstBoardList->setColumnText(1, QObject::tr("URL"));
    QString categoryItem = "2ch";
    bool result = mBoardParser.parse(mpHttpAccessor->getReceivedData(),
                                     categoryItem,
                                     lstBoardList);
    if (result == false)
    {
        // parsing failure... 
        mainTabWidget->setTabEnabled(BoardList, false);
        mainTabWidget->setCurrentPage(0);
    }
    lstBoardList->show();
    return;
}

// get bbs list request
void MainApp::getBBSlistRequest()
{
    ///// 
    if (mpHttpAccessor == 0)
    {
        return;
    }

    // send to request
    QString    referer = "";
    QByteArray value(0);
    bool result = mpHttpAccessor->getDataFromURL(rootj::HTTP_METHOD_GET, 
                                                 ROOTJ_HTTP_BOARDLIST,
                                                 edtBbsListUrl->text(),
                                                 referer,
                                                 value);
    if (result != true)
    {
        QMessageBox::critical(NULL,QObject::tr("error"),QObject::tr("HTTP communication error."),QMessageBox::Ok, QMessageBox::NoButton);
    }
    return;
}

// selected board...
void MainApp::selectedBoard(QListViewItem *apItem)
{
    // set selected BBS item
    mpSelectedBBS = apItem;

    // create URL
    QString subjectListURL = apItem->text(1) + "subject.txt";
    
    ///// 
    if (mpHttpAccessor == 0)
    {
        return;
    }

    // send to request
    QString    referer = "";
    QByteArray value(0);
    bool result = mpHttpAccessor->getDataFromURL(rootj::HTTP_METHOD_GET,
                                                 ROOTJ_HTTP_THREADLIST,
                                                 subjectListURL,
                                                 referer,
                                                 value);
    if (result != true)
    {
        QMessageBox::critical(NULL,QObject::tr("error"),QObject::tr("HTTP communication error."),QMessageBox::Ok, QMessageBox::NoButton);
    }
    return;    
}


void MainApp::updateSubjectList()
{
    ///// 
    if (mpHttpAccessor == 0)
    {
        return;
    }

    // enables
    mainTabWidget->setTabEnabled(ThreadList, true);
    mainTabWidget->setCurrentPage(2);

    // sets a title header
    QHeader  *header = threadList->horizontalHeader();
    header->resizeSection(0, 430);
    header->resizeSection(1, 35);
    header->setLabel(0, tr("Title list : ") + mpSelectedBBS->text(0));
    header->setLabel(1, tr("Mes."));
    header->setLabel(2, tr("dat file name"));


    // sets the header...
    int rows = threadList->numRows();
    for (int loopCount = 0; loopCount < rows; loopCount++)
    {
        threadList->removeRow(0);
    }

    // parse threads
    bool result = mThreadParser.parse(mpHttpAccessor->getReceivedData(),
                                      threadList);
    if (result != true)
    {
        // parsing failure... 
        mainTabWidget->setTabEnabled(BoardList, false);
        mainTabWidget->setCurrentPage(1);
    }
    return;
}

void MainApp::slot_finishedHttpCommunication( int aTransactionId )
{
    //QMessageBox::information(NULL,QObject::tr("information"),QObject::tr("received SIGNAL (finishedHttpCommunication())"),QMessageBox::Ok);

   switch (aTransactionId)
    {
      case ROOTJ_HTTP_THREADLIST:
        updateSubjectList();
        break;
        
      case ROOTJ_HTTP_BOARDLIST:
        update_BBSlist();
        break;

      default:
        break;
    }
    
    return;
}

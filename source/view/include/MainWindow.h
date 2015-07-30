#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ViewApi.h"

class QAction;
class LogWindow;
class AudioUnitsManagerWindow;
class AudioUnitPropertiesWindow;
class SignalChainWidget;

class QMUSIC_VIEW_API MainWindow : public QMainWindow
{
    Q_OBJECT
public:

    MainWindow(QWidget *pParent = nullptr, Qt::WindowFlags flags = 0);
    ~MainWindow();

private slots:

    void newSignalChain();
    void saveSignalChain();
    void saveAsSignalChain();

    void startSignalChain();
    void stopSignalChain();

private:

    void createDockingWindows();
    void createActions();
    void createMenu();
    void createToolBars();

    void updateActions();

    LogWindow *m_pLogWindow;    
    AudioUnitsManagerWindow *m_pAudioUnitsManagerWindow;
    AudioUnitPropertiesWindow *m_pAudioUnitPropertiesWindow;

    /// Central widget showing a signal chain.
    SignalChainWidget *m_pSignalChainWidget;

    // Actions
    QAction *m_pNewSignalChainAction;
    QAction *m_pOpenSignalChainAction;
    QAction *m_pSaveSignalChainAction;
    QAction *m_pSaveAsSignalChainAction;
    QAction *m_pQuitAction;

    QAction *m_pStartSignalChainAction;
    QAction *m_pStopSignalChainAction;

    // Menus
    QMenu *m_pFileMenu;
    QMenu *m_pSoundMenu;

    // Toolbars
    QToolBar *m_pSignalChainToolBar;
};

#endif // MAINWINDOW_H


import React, { useState, useEffect } from 'react';
import { motion, AnimatePresence } from 'framer-motion';
import Desktop from './components/Desktop';
import BootScreen from './components/BootScreen';
import LoginScreen from './components/LoginScreen';
import { DesktopProvider } from './contexts/DesktopContext';
import { NotificationProvider } from './contexts/NotificationContext';
import { WorkspaceProvider } from './contexts/WorkspaceContext';
import { FileSystemProvider } from './hooks/useFileSystem';
import ScreenshotTool from './components/ScreenshotTool';

type SystemState = 'booting' | 'login' | 'desktop' | 'locked';

const App: React.FC = () => {
  const [systemState, setSystemState] = useState<SystemState>('booting');
  const [isTakingScreenshot, setIsTakingScreenshot] = useState(false);

  const handleLogin = () => {
    setSystemState('desktop');
  };
  
  const handleLock = () => {
    setSystemState('locked');
  };

  const handleLogout = () => {
    setSystemState('login');
  };

  useEffect(() => {
    const bootTimer = setTimeout(() => {
      setSystemState('login');
    }, 3000);

    const handleScreenshotRequest = () => setIsTakingScreenshot(true);
    window.addEventListener('request-screenshot', handleScreenshotRequest);

    return () => {
      clearTimeout(bootTimer);
      window.removeEventListener('request-screenshot', handleScreenshotRequest);
    };
  }, []);

  const renderContent = () => {
    switch (systemState) {
      case 'booting':
        return (
          <motion.div key="boot" initial={{ opacity: 0 }} animate={{ opacity: 1 }} exit={{ opacity: 0 }} transition={{ duration: 0.5 }} className="w-full h-full">
            <BootScreen />
          </motion.div>
        );
      case 'login':
      case 'locked':
        return (
          <motion.div key={systemState} initial={{ opacity: 0 }} animate={{ opacity: 1 }} exit={{ opacity: 0 }} transition={{ duration: 0.5 }} className="w-full h-full">
            <LoginScreen onLogin={handleLogin} isLockScreen={systemState === 'locked'} />
          </motion.div>
        );
      case 'desktop':
        return (
          <motion.div key="desktop" initial={{ opacity: 0 }} animate={{ opacity: 1 }} transition={{ duration: 0.5 }} className="w-full h-full">
            <FileSystemProvider>
              <WorkspaceProvider>
                <DesktopProvider>
                  <NotificationProvider>
                    <Desktop onLogout={handleLogout} onLock={handleLock} />
                  </NotificationProvider>
                </DesktopProvider>
              </WorkspaceProvider>
            </FileSystemProvider>
          </motion.div>
        );
      default:
        return null;
    }
  };

  return (
    <div className="w-full h-full bg-black overflow-hidden" id="desktop-container">
      <AnimatePresence mode="wait">
        {renderContent()}
      </AnimatePresence>
      {isTakingScreenshot && <ScreenshotTool onDone={() => setIsTakingScreenshot(false)} />}
    </div>
  );
};

export default App;

import React from 'react';
import { AnimatePresence } from 'framer-motion';
import Window from './Window';
import { useDesktop } from '../contexts/DesktopContext';

const WindowManager: React.FC = () => {
  const { currentWorkspaceWindows } = useDesktop();

  return (
    <div className="absolute inset-0 z-20 pointer-events-none">
      <AnimatePresence>
        {currentWorkspaceWindows.map((window) => (
          <Window key={window.id} window={window} />
        ))}
      </AnimatePresence>
    </div>
  );
};

export default WindowManager;

import React, { useState, useEffect } from 'react';
import { motion, AnimatePresence } from 'framer-motion';
import { useDesktop } from '../contexts/DesktopContext';
import { Monitor, FileText, Settings, Calculator, Folder, Terminal } from 'lucide-react';

interface WindowSwitcherProps {
  onClose: () => void;
}

const iconMap: { [key: string]: React.ElementType } = {
  SystemMonitor: Monitor,
  TextEditor: FileText,
  Settings: Settings,
  Calculator: Calculator,
  FileManager: Folder,
  Terminal: Terminal,
};

const WindowSwitcher: React.FC<WindowSwitcherProps> = ({ onClose }) => {
  const { windows, bringToFront } = useDesktop();
  const openWindows = windows.filter(w => !w.isMinimized);
  const [selectedIndex, setSelectedIndex] = useState(0);

  useEffect(() => {
    const handleKeyDown = (e: KeyboardEvent) => {
      if (e.key === 'Tab' && e.altKey) {
        e.preventDefault();
        setSelectedIndex(prev => (prev + 1) % openWindows.length);
      }
    };

    const handleKeyUp = (e: KeyboardEvent) => {
      if (e.key === 'Alt') {
        if (openWindows[selectedIndex]) {
          bringToFront(openWindows[selectedIndex].id);
        }
        onClose();
      }
    };

    window.addEventListener('keydown', handleKeyDown);
    window.addEventListener('keyup', handleKeyUp);

    return () => {
      window.removeEventListener('keydown', handleKeyDown);
      window.removeEventListener('keyup', handleKeyUp);
    };
  }, [openWindows, selectedIndex, bringToFront, onClose]);

  if (openWindows.length === 0) {
    onClose();
    return null;
  }

  return (
    <motion.div
      initial={{ opacity: 0, scale: 0.9 }}
      animate={{ opacity: 1, scale: 1 }}
      exit={{ opacity: 0, scale: 0.9 }}
      className="fixed inset-0 bg-black/50 backdrop-blur-md flex items-center justify-center z-[9998]"
    >
      <div className="bg-gray-800/80 p-4 rounded-xl border border-gray-600">
        <h2 className="text-white text-center text-lg mb-4">Open Applications</h2>
        <div className="flex flex-wrap gap-4 justify-center max-w-2xl">
          <AnimatePresence>
            {openWindows.map((window, index) => {
              const Icon = iconMap[window.component] || Monitor;
              return (
                <motion.div
                  key={window.id}
                  initial={{ opacity: 0, y: 20 }}
                  animate={{ opacity: 1, y: 0 }}
                  exit={{ opacity: 0, y: -20 }}
                  transition={{ delay: index * 0.05 }}
                  className={`p-4 rounded-lg w-32 h-28 flex flex-col items-center justify-center cursor-pointer transition-colors ${
                    index === selectedIndex ? 'bg-blue-500' : 'bg-gray-700/50'
                  }`}
                  onClick={() => {
                    bringToFront(window.id);
                    onClose();
                  }}
                >
                  <Icon className="h-10 w-10 text-white mb-2" />
                  <p className="text-white text-sm text-center truncate w-full">{window.title}</p>
                </motion.div>
              );
            })}
          </AnimatePresence>
        </div>
      </div>
    </motion.div>
  );
};

export default WindowSwitcher;

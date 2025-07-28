import React, { useState, useEffect } from 'react';
import { motion } from 'framer-motion';
import { 
  Menu, 
  Wifi, 
  Volume2, 
  Battery, 
  Bot,
  Terminal as TerminalIcon,
  Folder,
  Bell,
  LayoutGrid
} from 'lucide-react';
import { useDesktop } from '../contexts/DesktopContext';
import { useNotifications } from '../contexts/NotificationContext';

interface TaskbarProps {
  onAIAssistantToggle: () => void;
  onShowDesktop: () => void;
  onNotificationToggle: () => void;
  onWorkspaceToggle: () => void;
}

const Taskbar: React.FC<TaskbarProps> = ({ onAIAssistantToggle, onShowDesktop, onNotificationToggle, onWorkspaceToggle }) => {
  const { currentWorkspaceWindows, openWindow, bringToFront, activeWindowId } = useDesktop();
  const { notifications } = useNotifications();
  const [currentTime, setCurrentTime] = useState(new Date());

  useEffect(() => {
    const timer = setInterval(() => setCurrentTime(new Date()), 1000);
    return () => clearInterval(timer);
  }, []);
  
  const formatTime = (date: Date) => date.toLocaleTimeString('en-US', { hour: '2-digit', minute: '2-digit', hour12: true });
  const formatDate = (date: Date) => date.toLocaleDateString('en-US', { month: 'short', day: 'numeric' });

  const handleAppClick = (appId: string) => {
    bringToFront(appId);
  };

  return (
    <motion.div
      initial={{ y: 100, opacity: 0 }}
      animate={{ y: 0, opacity: 1 }}
      transition={{ duration: 0.5, delay: 0.2 }}
      className="absolute bottom-0 left-0 right-0 h-12 bg-gray-900/80 backdrop-blur-md border-t border-gray-700/50 flex items-center justify-between px-2 z-30"
    >
      <div className="flex items-center space-x-1">
        <motion.button whileHover={{ scale: 1.05 }} whileTap={{ scale: 0.95 }} onClick={() => openWindow({ title: 'Start Menu', component: 'StartMenu', position: { x: 10, y: window.innerHeight - 550 }, size: { width: 450, height: 500 } })} className="p-2 hover:bg-gray-700/50 rounded-lg transition-colors" title="Start Menu">
          <Menu className="h-5 w-5 text-white" />
        </motion.button>
        <motion.button whileHover={{ scale: 1.05 }} whileTap={{ scale: 0.95 }} onClick={() => openWindow({ title: 'Terminal', component: 'Terminal', position: { x: 100, y: 100 }, size: { width: 600, height: 400 } })} className="p-2 hover:bg-gray-700/50 rounded-lg transition-colors" title="Terminal">
          <TerminalIcon className="h-5 w-5 text-gray-300" />
        </motion.button>
        <motion.button whileHover={{ scale: 1.05 }} whileTap={{ scale: 0.95 }} onClick={() => openWindow({ title: 'File Manager', component: 'FileManager', position: { x: 150, y: 50 }, size: { width: 700, height: 500 } })} className="p-2 hover:bg-gray-700/50 rounded-lg transition-colors" title="File Manager">
          <Folder className="h-5 w-5 text-blue-400" />
        </motion.button>
        <motion.button whileHover={{ scale: 1.05 }} whileTap={{ scale: 0.95 }} onClick={onWorkspaceToggle} className="p-2 hover:bg-gray-700/50 rounded-lg transition-colors" title="Task View (Win+Tab)">
          <LayoutGrid className="h-5 w-5 text-gray-300" />
        </motion.button>
      </div>

      <div className="flex-1 flex items-center justify-center space-x-1 mx-4 overflow-hidden">
        {currentWorkspaceWindows.filter(w => !w.isMinimized).map((window) => (
          <motion.button
            key={window.id}
            layout
            whileHover={{ scale: 1.05 }}
            whileTap={{ scale: 0.95 }}
            onClick={() => handleAppClick(window.id)}
            className={`px-3 py-1 rounded text-white text-sm truncate max-w-32 transition-colors ${activeWindowId === window.id ? 'bg-gray-600/70' : 'bg-gray-700/50 hover:bg-gray-600/50'}`}
          >
            {window.title}
          </motion.button>
        ))}
      </div>

      <div className="flex items-center space-x-1">
        <motion.button whileHover={{ scale: 1.05 }} whileTap={{ scale: 0.95 }} onClick={onAIAssistantToggle} className="p-2 hover:bg-gray-700/50 rounded-lg transition-colors" title="AI Assistant (Ctrl+Alt+A)">
          <Bot className="h-4 w-4 text-blue-400" />
        </motion.button>
        <div className="flex items-center space-x-1 text-gray-400 p-2">
          <Wifi className="h-4 w-4" />
          <Volume2 className="h-4 w-4" />
          <Battery className="h-4 w-4" />
        </div>
        <motion.button whileHover={{ scale: 1.05 }} whileTap={{ scale: 0.95 }} onClick={onNotificationToggle} className="p-2 hover:bg-gray-700/50 rounded-lg transition-colors relative" title="Notifications">
          <Bell className="h-4 w-4 text-gray-400" />
          {notifications.length > 0 && (
            <span className="absolute top-1 right-1 block h-2 w-2 rounded-full bg-red-500" />
          )}
        </motion.button>
        <div className="text-white text-sm font-medium px-2 text-center">
          <div>{formatTime(currentTime)}</div>
          <div className="text-xs text-gray-400">{formatDate(currentTime)}</div>
        </div>
        <button onClick={onShowDesktop} className="w-2 h-full hover:bg-gray-700/50" title="Show Desktop (Win+D)" />
      </div>
    </motion.div>
  );
};

export default Taskbar;

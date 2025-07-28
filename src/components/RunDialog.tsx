import React, { useState } from 'react';
import { motion } from 'framer-motion';
import { useDesktop } from '../contexts/DesktopContext';
import { Terminal } from 'lucide-react';

interface RunDialogProps {
  onClose: () => void;
}

const RunDialog: React.FC<RunDialogProps> = ({ onClose }) => {
  const [command, setCommand] = useState('');
  const { openWindow } = useDesktop();

  const handleRun = () => {
    const cmd = command.toLowerCase().trim();
    const apps: { [key: string]: any } = {
      'terminal': { title: 'Terminal', component: 'Terminal', size: { width: 600, height: 400 } },
      'calc': { title: 'Calculator', component: 'Calculator', size: { width: 320, height: 450 } },
      'notepad': { title: 'Text Editor', component: 'TextEditor', size: { width: 600, height: 400 } },
      'settings': { title: 'Settings', component: 'Settings', size: { width: 500, height: 400 } },
      'explorer': { title: 'File Manager', component: 'FileManager', size: { width: 700, height: 500 } },
    };

    if (apps[cmd]) {
      openWindow({ ...apps[cmd], position: { x: 200, y: 150 } });
      onClose();
    } else {
      alert(`Command not found: ${cmd}`);
    }
  };

  return (
    <motion.div
      initial={{ opacity: 0, y: -20 }}
      animate={{ opacity: 1, y: 0 }}
      exit={{ opacity: 0, y: -20 }}
      className="absolute top-1/2 left-1/2 -translate-x-1/2 -translate-y-1/2 z-50 bg-white dark:bg-gray-800 rounded-lg shadow-xl p-6 w-96 border border-gray-300 dark:border-gray-600"
    >
      <div className="flex items-start mb-4">
        <Terminal className="h-8 w-8 text-blue-500 mr-4" />
        <div>
          <h2 className="text-lg font-semibold text-gray-900 dark:text-gray-100">Run Command</h2>
          <p className="text-sm text-gray-600 dark:text-gray-400">Type the name of a program to open it.</p>
        </div>
      </div>
      <div className="flex items-center space-x-2 mb-6">
        <label htmlFor="command" className="font-medium text-gray-700 dark:text-gray-300">Open:</label>
        <input
          id="command"
          type="text"
          value={command}
          onChange={(e) => setCommand(e.target.value)}
          onKeyPress={(e) => e.key === 'Enter' && handleRun()}
          className="flex-1 px-3 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:outline-none focus:ring-2 focus:ring-blue-500 dark:bg-gray-700 dark:text-white"
          autoFocus
        />
      </div>
      <div className="flex justify-end space-x-2">
        <button onClick={handleRun} className="px-4 py-2 bg-blue-500 text-white rounded-lg hover:bg-blue-600">Run</button>
        <button onClick={onClose} className="px-4 py-2 bg-gray-200 dark:bg-gray-600 text-gray-800 dark:text-gray-200 rounded-lg hover:bg-gray-300 dark:hover:bg-gray-500">Cancel</button>
      </div>
    </motion.div>
  );
};

export default RunDialog;

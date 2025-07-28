import React, { useState, useEffect } from 'react';
import { motion } from 'framer-motion';
import { Sun, Moon, Palette, Monitor, User, MousePointer, Bell } from 'lucide-react';

const SettingsApp: React.FC = () => {
  const [theme, setTheme] = useState('dark');
  const [hotCorners, setHotCorners] = useState({
    'top-left': 'workspaces',
    'top-right': 'notifications',
    'bottom-left': 'start-menu',
    'bottom-right': 'show-desktop',
  });
  
  useEffect(() => {
    const root = window.document.documentElement;
    if (theme === 'dark') {
      root.classList.add('dark');
    } else {
      root.classList.remove('dark');
    }
  }, [theme]);

  const sections = [
    { name: 'Appearance', icon: Palette },
    { name: 'Display', icon: Monitor },
    { name: 'Account', icon: User },
    { name: 'Hot Corners', icon: MousePointer },
    { name: 'Notifications', icon: Bell },
  ];
  const [activeSection, setActiveSection] = useState('Appearance');

  const renderContent = () => {
    switch (activeSection) {
      case 'Appearance':
        return (
          <div>
            <h3 className="text-lg font-semibold mb-4">Theme</h3>
            <div className="flex space-x-4">
              <motion.button whileTap={{ scale: 0.95 }} onClick={() => setTheme('light')} className={`p-4 border rounded-lg flex-1 ${theme === 'light' ? 'border-blue-500 bg-blue-50 dark:bg-blue-900/20' : 'border-gray-300 dark:border-gray-600'}`}>
                <Sun className="mx-auto mb-2" /> Light
              </motion.button>
              <motion.button whileTap={{ scale: 0.95 }} onClick={() => setTheme('dark')} className={`p-4 border rounded-lg flex-1 ${theme === 'dark' ? 'border-blue-500 bg-blue-50 dark:bg-blue-900/20' : 'border-gray-300 dark:border-gray-600'}`}>
                <Moon className="mx-auto mb-2" /> Dark
              </motion.button>
            </div>
          </div>
        );
      case 'Hot Corners':
        return (
          <div>
            <h3 className="text-lg font-semibold mb-4">Configure Hot Corners</h3>
            <p className="text-sm text-gray-500 mb-4">Assign actions to the corners of your screen.</p>
            <div className="grid grid-cols-2 gap-4">
              {Object.keys(hotCorners).map(corner => (
                <div key={corner}>
                  <label className="block text-sm font-medium capitalize mb-1">{corner.replace('-', ' ')}</label>
                  <select className="w-full p-2 border rounded-lg dark:bg-gray-800">
                    <option>Show Workspaces</option>
                    <option>Show Notifications</option>
                    <option>Show Desktop</option>
                    <option>Open Start Menu</option>
                    <option>None</option>
                  </select>
                </div>
              ))}
            </div>
          </div>
        );
      default:
        return <p>Settings for {activeSection} would be here.</p>;
    }
  };

  return (
    <div className="h-full flex bg-white dark:bg-gray-900 text-gray-900 dark:text-gray-100">
      <div className="w-56 p-4 border-r border-gray-200 dark:border-gray-700">
        <h2 className="text-xl font-bold mb-6">Settings</h2>
        <nav className="space-y-1">
          {sections.map(section => (
            <button key={section.name} onClick={() => setActiveSection(section.name)} className={`w-full flex items-center p-2 rounded-lg text-left transition-colors ${activeSection === section.name ? 'bg-blue-500 text-white' : 'hover:bg-gray-100 dark:hover:bg-gray-800'}`}>
              <section.icon className="mr-3" size={20} />
              {section.name}
            </button>
          ))}
        </nav>
      </div>
      <div className="flex-1 p-6 overflow-y-auto">
        {renderContent()}
      </div>
    </div>
  );
};

export default SettingsApp;

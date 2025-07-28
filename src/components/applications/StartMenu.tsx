import React, { useState } from 'react';
import { motion } from 'framer-motion';
import { Search, Power, Settings, User, Monitor, FileText, Calculator, Terminal, Folder, Image, Music, Video, Globe, Mail, Calendar, Star, Package } from 'lucide-react';
import { useDesktop } from '../../contexts/DesktopContext';

const StartMenu: React.FC = () => {
  const [searchTerm, setSearchTerm] = useState('');
  const { openWindow, closeWindow } = useDesktop();

  const applications = [
    { name: 'Terminal', icon: Terminal, component: 'Terminal', category: 'System', size: { width: 600, height: 400 } },
    { name: 'File Manager', icon: Folder, component: 'FileManager', category: 'System', size: { width: 700, height: 500 } },
    { name: 'System Monitor', icon: Monitor, component: 'SystemMonitor', category: 'System', size: { width: 600, height: 450 } },
    { name: 'Text Editor', icon: FileText, component: 'TextEditor', category: 'Productivity', size: { width: 600, height: 400 } },
    { name: 'Calculator', icon: Calculator, component: 'Calculator', category: 'Utilities', size: { width: 320, height: 450 } },
    { name: 'Settings', icon: Settings, component: 'Settings', category: 'System', size: { width: 500, height: 400 } },
    { name: 'Package Manager', icon: Package, component: 'PackageManager', category: 'System', size: { width: 700, height: 500 } },
    { name: 'Image Viewer', icon: Image, component: 'ImageViewer', category: 'Media', size: { width: 800, height: 600 } },
    { name: 'Web Browser', icon: Globe, component: 'WebBrowser', category: 'Internet', size: { width: 1024, height: 768 } },
    { name: 'Calendar', icon: Calendar, component: 'Calendar', category: 'Productivity', size: { width: 800, height: 600 } },
  ];

  const recommendedApps = ['File Manager', 'Web Browser', 'Terminal', 'Settings'];

  const filteredApps = applications.filter(app => 
    app.name.toLowerCase().includes(searchTerm.toLowerCase())
  );

  const handleAppClick = (app: any) => {
    openWindow({
      title: app.name,
      component: app.component,
      position: { x: Math.random() * 200 + 100, y: Math.random() * 100 + 50 },
      size: app.size
    });
    closeWindow('start-menu');
  };

  return (
    <div className="h-full bg-white/80 dark:bg-gray-900/80 backdrop-blur-xl flex flex-col text-gray-900 dark:text-gray-100">
      <div className="p-4">
        <div className="relative">
          <Search className="absolute left-3 top-1/2 -translate-y-1/2 h-4 w-4 text-gray-400" />
          <input type="text" placeholder="Search applications..." value={searchTerm} onChange={(e) => setSearchTerm(e.target.value)} className="w-full pl-10 pr-4 py-2 border border-gray-300 dark:border-gray-600 rounded-lg bg-white/50 dark:bg-gray-800/50 focus:outline-none focus:ring-2 focus:ring-blue-500" />
        </div>
      </div>
      
      {searchTerm === '' && (
        <div className="p-4 border-t border-gray-200 dark:border-gray-700">
          <h3 className="text-sm font-semibold text-gray-500 dark:text-gray-400 mb-2">Recommended</h3>
          <div className="grid grid-cols-2 gap-2">
            {applications.filter(app => recommendedApps.includes(app.name)).map(app => (
              <motion.button key={app.name} whileHover={{ scale: 1.02 }} whileTap={{ scale: 0.98 }} onClick={() => handleAppClick(app)} className="flex items-center p-3 rounded-lg text-left hover:bg-black/5 dark:hover:bg-white/5">
                <div className="w-10 h-10 bg-blue-500/20 rounded-lg flex items-center justify-center mr-3"><app.icon className="h-5 w-5 text-blue-500" /></div>
                <div className="font-medium">{app.name}</div>
              </motion.button>
            ))}
          </div>
        </div>
      )}

      <div className="flex-1 overflow-auto p-4 border-t border-gray-200 dark:border-gray-700">
        <h3 className="text-sm font-semibold text-gray-500 dark:text-gray-400 mb-2">{searchTerm ? 'Search Results' : 'All Apps'}</h3>
        <div className="grid grid-cols-1 gap-1">
          {filteredApps.map((app, index) => (
            <motion.button key={app.name} initial={{ opacity: 0, x: -20 }} animate={{ opacity: 1, x: 0 }} transition={{ delay: index * 0.03 }} whileHover={{ backgroundColor: 'rgba(59, 130, 246, 0.1)' }} whileTap={{ scale: 0.98 }} onClick={() => handleAppClick(app)} className="flex items-center p-2 rounded-lg text-left">
              <div className="w-8 h-8 bg-gray-500/10 rounded-lg flex items-center justify-center mr-3"><app.icon className="h-4 w-4 text-gray-600 dark:text-gray-300" /></div>
              <div className="font-medium text-sm">{app.name}</div>
            </motion.button>
          ))}
        </div>
        {filteredApps.length === 0 && <div className="text-center text-gray-500 mt-8"><Search className="h-16 w-16 mx-auto mb-4 opacity-50" /><p>No applications found</p></div>}
      </div>
      <div className="p-3 border-t border-gray-200 dark:border-gray-700 mt-auto">
        <div className="flex justify-between items-center">
          <motion.button whileHover={{ scale: 1.05 }} whileTap={{ scale: 0.95 }} className="flex items-center p-2 hover:bg-black/5 dark:hover:bg-white/5 rounded-lg">
            <div className="w-8 h-8 bg-blue-500 rounded-full flex items-center justify-center mr-2"><User className="h-4 w-4 text-white" /></div>
            User
          </motion.button>
          <motion.button whileHover={{ scale: 1.05 }} whileTap={{ scale: 0.95 }} className="p-2 text-gray-600 dark:text-gray-300 hover:bg-black/5 dark:hover:bg-white/5 rounded-lg"><Power className="h-5 w-5" /></motion.button>
        </div>
      </div>
    </div>
  );
};

export default StartMenu;

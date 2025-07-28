import React from 'react';
import { motion } from 'framer-motion';
import { 
  Monitor, 
  FileText, 
  Settings, 
  Calculator,
  Folder,
  Package
} from 'lucide-react';
import { useDesktop } from '../contexts/DesktopContext';

const DesktopIcons: React.FC = () => {
  const { openWindow } = useDesktop();

  const icons = [
    {
      name: 'File Manager', icon: Folder,
      onClick: () => openWindow({ title: 'File Manager', component: 'FileManager', position: { x: 150, y: 50 }, size: { width: 700, height: 500 } })
    },
    {
      name: 'System Monitor', icon: Monitor,
      onClick: () => openWindow({ title: 'System Monitor', component: 'SystemMonitor', position: { x: 200, y: 100 }, size: { width: 600, height: 450 } })
    },
    {
      name: 'Text Editor', icon: FileText,
      onClick: () => openWindow({ title: 'Text Editor', component: 'TextEditor', position: { x: 100, y: 150 }, size: { width: 600, height: 400 } })
    },
    {
      name: 'Calculator', icon: Calculator,
      onClick: () => openWindow({ title: 'Calculator', component: 'Calculator', position: { x: 300, y: 200 }, size: { width: 320, height: 450 } })
    },
    {
      name: 'Settings', icon: Settings,
      onClick: () => openWindow({ title: 'Settings', component: 'Settings', position: { x: 250, y: 150 }, size: { width: 550, height: 400 } })
    },
    {
      name: 'Packages', icon: Package,
      onClick: () => openWindow({ title: 'Package Manager', component: 'PackageManager', position: { x: 220, y: 120 }, size: { width: 700, height: 500 } })
    },
  ];

  return (
    <div className="absolute top-8 left-8 flex flex-col gap-4">
      {icons.map((icon, index) => (
        <motion.div
          key={icon.name}
          initial={{ opacity: 0, scale: 0.8 }}
          animate={{ opacity: 1, scale: 1 }}
          transition={{ duration: 0.3, delay: index * 0.1 }}
          className="pointer-events-auto"
        >
          <motion.button
            whileHover={{ scale: 1.1 }}
            whileTap={{ scale: 0.95 }}
            onDoubleClick={icon.onClick}
            className="flex flex-col items-center p-2 rounded-lg hover:bg-white/10 transition-colors group w-24"
          >
            <div className="w-12 h-12 bg-gradient-to-br from-blue-500 to-purple-600 rounded-xl flex items-center justify-center mb-2 group-hover:from-blue-400 group-hover:to-purple-500 transition-colors shadow-lg">
              <icon.icon className="h-6 w-6 text-white" />
            </div>
            <span className="text-white text-xs font-medium text-center max-w-full truncate text-shadow-sm">
              {icon.name}
            </span>
          </motion.button>
        </motion.div>
      ))}
    </div>
  );
};

export default DesktopIcons;

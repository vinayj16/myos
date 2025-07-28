import React, { useState } from 'react';
import { motion } from 'framer-motion';
import { Package, Search, Download, Trash2, CheckCircle } from 'lucide-react';

interface AppPackage {
  id: string;
  name: string;
  description: string;
  version: string;
  category: string;
  installed: boolean;
}

const availablePackages: AppPackage[] = [
  { id: 'vscode', name: 'Code Editor', description: 'A powerful and lightweight code editor.', version: '1.85.1', category: 'Development', installed: true },
  { id: 'figma', name: 'Figma', description: 'Collaborative interface design tool.', version: '116.8.6', category: 'Design', installed: false },
  { id: 'slack', name: 'Slack', description: 'Team communication and collaboration.', version: '4.35.131', category: 'Productivity', installed: false },
  { id: 'spotify', name: 'Spotify', description: 'Digital music service.', version: '1.2.29', category: 'Music', installed: true },
  { id: 'blender', name: 'Blender', description: 'Free and open-source 3D creation suite.', version: '4.0.2', category: 'Graphics', installed: false },
  { id: 'obs', name: 'OBS Studio', description: 'Software for video recording and live streaming.', version: '30.0.0', category: 'Streaming', installed: false },
];

const PackageManager: React.FC = () => {
  const [packages, setPackages] = useState<AppPackage[]>(availablePackages);
  const [searchTerm, setSearchTerm] = useState('');

  const toggleInstall = (id: string) => {
    setPackages(prev =>
      prev.map(pkg =>
        pkg.id === id ? { ...pkg, installed: !pkg.installed } : pkg
      )
    );
  };

  const filteredPackages = packages.filter(pkg =>
    pkg.name.toLowerCase().includes(searchTerm.toLowerCase()) ||
    pkg.description.toLowerCase().includes(searchTerm.toLowerCase())
  );

  return (
    <div className="h-full flex flex-col bg-gray-50 dark:bg-gray-900 text-gray-900 dark:text-gray-100">
      <div className="p-4 border-b border-gray-200 dark:border-gray-700">
        <div className="flex justify-between items-center">
          <div className="flex items-center">
            <Package size={24} className="text-blue-500 mr-3" />
            <h1 className="text-xl font-bold">Package Manager</h1>
          </div>
          <div className="relative">
            <Search className="absolute left-3 top-1/2 -translate-y-1/2 h-4 w-4 text-gray-400" />
            <input
              type="text"
              placeholder="Search packages..."
              value={searchTerm}
              onChange={(e) => setSearchTerm(e.target.value)}
              className="pl-10 pr-4 py-2 border rounded-lg dark:bg-gray-800"
            />
          </div>
        </div>
      </div>
      <div className="flex-1 overflow-y-auto">
        <div className="p-4 grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4">
          {filteredPackages.map((pkg, index) => (
            <motion.div
              key={pkg.id}
              initial={{ opacity: 0, y: 20 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ delay: index * 0.05 }}
              className="bg-white dark:bg-gray-800 rounded-lg p-4 border border-gray-200 dark:border-gray-700 flex flex-col justify-between"
            >
              <div>
                <h2 className="text-lg font-semibold">{pkg.name}</h2>
                <p className="text-sm text-gray-500 dark:text-gray-400 mb-2">{pkg.category}</p>
                <p className="text-sm text-gray-600 dark:text-gray-300 mb-4">{pkg.description}</p>
              </div>
              <div className="flex justify-between items-center mt-auto">
                <span className="text-xs text-gray-400">v{pkg.version}</span>
                {pkg.installed ? (
                  <button
                    onClick={() => toggleInstall(pkg.id)}
                    className="flex items-center px-3 py-1 bg-red-500/10 text-red-500 rounded hover:bg-red-500/20 text-sm"
                  >
                    <Trash2 size={14} className="mr-1" /> Uninstall
                  </button>
                ) : (
                  <button
                    onClick={() => toggleInstall(pkg.id)}
                    className="flex items-center px-3 py-1 bg-blue-500/10 text-blue-500 rounded hover:bg-blue-500/20 text-sm"
                  >
                    <Download size={14} className="mr-1" /> Install
                  </button>
                )}
              </div>
            </motion.div>
          ))}
        </div>
      </div>
    </div>
  );
};

export default PackageManager;

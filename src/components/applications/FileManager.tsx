import React, { useState, useEffect } from 'react';
import { motion } from 'framer-motion';
import { Folder, File, Home, ChevronLeft, Search, Grid, List, FolderPlus, FilePlus, Trash2, Sparkles } from 'lucide-react';
import { useFileSystem, FileNode } from '../../hooks/useFileSystem';

const FileManager: React.FC = () => {
  const { getNode, createFile, createFolder, deleteNode, updateAccessTime } = useFileSystem();
  const [currentPath, setCurrentPath] = useState('/home/user');
  const [viewMode, setViewMode] = useState<'list' | 'list'>('list');
  const [searchTerm, setSearchTerm] = useState('');
  const [files, setFiles] = useState<FileNode[]>([]);
  const [sortMode, setSortMode] = useState<'name' | 'ai'>('name');

  useEffect(() => {
    const node = getNode(currentPath);
    if (node && node.type === 'folder' && node.children) {
      let childrenArray = Object.values(node.children);
      
      if (sortMode === 'ai') {
        childrenArray.sort((a, b) => (b.lastAccessed || 0) - (a.lastAccessed || 0));
      } else {
        childrenArray.sort((a, b) => a.name.localeCompare(b.name));
      }

      const filtered = childrenArray.filter(file =>
        file.name.toLowerCase().includes(searchTerm.toLowerCase())
      );
      setFiles(filtered);
    }
  }, [currentPath, getNode, searchTerm, sortMode]);

  const handleItemClick = (item: FileNode) => {
    const itemPath = currentPath === '/' ? `/${item.name}` : `${currentPath}/${item.name}`;
    updateAccessTime(itemPath);
    if (item.type === 'folder') {
      setCurrentPath(itemPath);
    }
  };

  const navigateUp = () => {
    if (currentPath === '/') return;
    const pathParts = currentPath.split('/').filter(Boolean);
    const newPath = '/' + pathParts.slice(0, -1).join('/');
    setCurrentPath(newPath || '/');
  };

  const handleCreateFolder = () => {
    const name = prompt('Enter folder name:');
    if (name) createFolder(currentPath, name);
  };
  
  const handleCreateFile = () => {
    const name = prompt('Enter file name:');
    if (name) createFile(currentPath, name);
  };
  
  const handleDelete = (name: string) => {
    if (window.confirm(`Are you sure you want to delete "${name}"?`)) {
      deleteNode(currentPath, name);
    }
  };

  return (
    <div className="h-full flex flex-col bg-white dark:bg-gray-900 text-gray-900 dark:text-gray-100">
      <div className="border-b border-gray-200 dark:border-gray-700 p-3">
        <div className="flex items-center justify-between mb-3">
          <div className="flex items-center space-x-2">
            <button onClick={navigateUp} className="p-2 hover:bg-gray-100 dark:hover:bg-gray-800 rounded"><ChevronLeft className="h-4 w-4" /></button>
            <button onClick={() => setCurrentPath('/home/user')} className="p-2 hover:bg-gray-100 dark:hover:bg-gray-800 rounded"><Home className="h-4 w-4" /></button>
            <button onClick={handleCreateFolder} className="p-2 hover:bg-gray-100 dark:hover:bg-gray-800 rounded"><FolderPlus className="h-4 w-4" /></button>
            <button onClick={handleCreateFile} className="p-2 hover:bg-gray-100 dark:hover:bg-gray-800 rounded"><FilePlus className="h-4 w-4" /></button>
          </div>
          <div className="flex items-center space-x-2">
            <button onClick={() => setSortMode(p => p === 'name' ? 'ai' : 'name')} className={`p-2 rounded flex items-center gap-1 ${sortMode === 'ai' ? 'text-blue-500' : ''}`}><Sparkles size={16} /> {sortMode === 'ai' ? 'Smart Sort' : 'Sort'}</button>
            <div className="relative">
              <Search className="absolute left-3 top-1/2 -translate-y-1/2 h-4 w-4 text-gray-400" />
              <input type="text" placeholder="Search..." value={searchTerm} onChange={(e) => setSearchTerm(e.target.value)} className="pl-10 pr-4 py-2 border rounded-lg dark:bg-gray-800" />
            </div>
            <div className="flex border rounded-lg overflow-hidden"><button onClick={() => setViewMode('list')} className={`p-2 ${viewMode === 'list' ? 'bg-blue-500 text-white' : 'hover:bg-gray-100 dark:hover:bg-gray-800'}`}><List className="h-4 w-4" /></button><button onClick={() => setViewMode('grid')} className={`p-2 ${viewMode === 'grid' ? 'bg-blue-500 text-white' : 'hover:bg-gray-100 dark:hover:bg-gray-800'}`}><Grid className="h-4 w-4" /></button></div>
          </div>
        </div>
        <div className="flex items-center bg-gray-100 dark:bg-gray-800 rounded px-3 py-1 text-sm"><span>{currentPath}</span></div>
      </div>
      <div className="flex-1 overflow-auto p-4">
        {viewMode === 'list' ? (
          <div className="space-y-1">
            {files.map((file, index) => (
              <motion.div key={file.name} initial={{ opacity: 0, y: 10 }} animate={{ opacity: 1, y: 0 }} transition={{ delay: index * 0.05 }} onDoubleClick={() => handleItemClick(file)} className="flex items-center p-2 rounded hover:bg-blue-50 dark:hover:bg-blue-900/20 cursor-pointer group">
                {file.type === 'folder' ? <Folder className="h-5 w-5 text-blue-500 mr-3" /> : <File className="h-5 w-5 text-gray-500 mr-3" />}
                <div className="flex-1 font-medium">{file.name}</div>
                <button onClick={() => handleDelete(file.name)} className="p-1 text-gray-400 hover:text-red-500 opacity-0 group-hover:opacity-100"><Trash2 size={16} /></button>
              </motion.div>
            ))}
          </div>
        ) : (
          <div className="grid grid-cols-4 gap-4">
            {files.map((file, index) => (
              <motion.div key={file.name} initial={{ opacity: 0, scale: 0.8 }} animate={{ opacity: 1, scale: 1 }} transition={{ delay: index * 0.05 }} onDoubleClick={() => handleItemClick(file)} className="flex flex-col items-center p-4 rounded-lg hover:bg-gray-100 dark:hover:bg-gray-800 cursor-pointer group relative">
                {file.type === 'folder' ? <Folder className="h-12 w-12 text-blue-500" /> : <File className="h-12 w-12 text-gray-500" />}
                <div className="text-sm text-center font-medium truncate w-full mt-2">{file.name}</div>
                <button onClick={() => handleDelete(file.name)} className="absolute top-1 right-1 p-1 text-gray-400 hover:text-red-500 opacity-0 group-hover:opacity-100"><Trash2 size={16} /></button>
              </motion.div>
            ))}
          </div>
        )}
        {files.length === 0 && <div className="text-center text-gray-500 mt-8"><Folder className="h-16 w-16 mx-auto mb-4 opacity-50" /><p>This folder is empty</p></div>}
      </div>
    </div>
  );
};

export default FileManager;

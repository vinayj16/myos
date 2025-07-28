import React, { useState, useCallback, createContext, useContext } from 'react';

export interface FileNode {
  name: string;
  type: 'file' | 'folder';
  children?: { [key: string]: FileNode };
  content?: string;
  lastAccessed?: number;
}

const initialFileSystem: FileNode = {
  name: 'root',
  type: 'folder',
  children: {
    home: {
      name: 'home',
      type: 'folder',
      children: {
        user: {
          name: 'user',
          type: 'folder',
          children: {
            Desktop: { name: 'Desktop', type: 'folder', children: {}, lastAccessed: Date.now() },
            Documents: {
              name: 'Documents',
              type: 'folder',
              children: {
                'report.txt': { name: 'report.txt', type: 'file', content: 'This is a financial report.', lastAccessed: Date.now() - 200000 },
              },
              lastAccessed: Date.now() - 100000
            },
            Downloads: { name: 'Downloads', type: 'folder', children: {}, lastAccessed: Date.now() - 500000 },
            'README.md': { name: 'README.md', type: 'file', content: '# Welcome to MyOS!', lastAccessed: Date.now() },
          },
        },
      },
    },
  },
};

const findNode = (path: string, fs: FileNode): FileNode | null => {
  const parts = path.split('/').filter(p => p);
  let currentNode: FileNode = fs;
  for (const part of parts) {
    if (currentNode.type === 'folder' && currentNode.children && currentNode.children[part]) {
      currentNode = currentNode.children[part];
    } else {
      return null;
    }
  }
  return currentNode;
};

interface FileSystemContextType {
  fs: FileNode;
  getNode: (path: string) => FileNode | null;
  createFile: (path: string, name: string) => void;
  createFolder: (path: string, name: string) => void;
  deleteNode: (path: string, name: string) => void;
  updateAccessTime: (path: string) => void;
}

const FileSystemContext = createContext<FileSystemContextType | undefined>(undefined);

export const useFileSystem = () => {
  const context = useContext(FileSystemContext);
  if (!context) {
    throw new Error('useFileSystem must be used within a FileSystemProvider');
  }
  return context;
};

export const FileSystemProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const [fs, setFs] = useState<FileNode>(initialFileSystem);

  const getNode = useCallback((path: string) => {
    return findNode(path, fs);
  }, [fs]);
  
  const updateAccessTime = useCallback((path: string) => {
    setFs(currentFs => {
      const newFs = JSON.parse(JSON.stringify(currentFs));
      const node = findNode(path, newFs);
      if (node) {
        node.lastAccessed = Date.now();
      }
      return newFs;
    });
  }, []);

  const createFile = useCallback((path: string, name: string) => {
    setFs(currentFs => {
      const newFs = JSON.parse(JSON.stringify(currentFs));
      const parentNode = findNode(path, newFs);
      if (parentNode && parentNode.type === 'folder' && parentNode.children) {
        if (!parentNode.children[name]) {
          parentNode.children[name] = { name, type: 'file', content: '', lastAccessed: Date.now() };
        }
      }
      return newFs;
    });
  }, []);

  const createFolder = useCallback((path: string, name: string) => {
    setFs(currentFs => {
      const newFs = JSON.parse(JSON.stringify(currentFs));
      const parentNode = findNode(path, newFs);
      if (parentNode && parentNode.type === 'folder' && parentNode.children) {
        if (!parentNode.children[name]) {
          parentNode.children[name] = { name, type: 'folder', children: {}, lastAccessed: Date.now() };
        }
      }
      return newFs;
    });
  }, []);

  const deleteNode = useCallback((path: string, name: string) => {
    setFs(currentFs => {
      const newFs = JSON.parse(JSON.stringify(currentFs));
      const parentNode = findNode(path, newFs);
      if (parentNode && parentNode.type === 'folder' && parentNode.children && parentNode.children[name]) {
        delete parentNode.children[name];
      }
      return newFs;
    });
  }, []);

  return (
    <FileSystemContext.Provider value={{ fs, getNode, createFile, createFolder, deleteNode, updateAccessTime }}>
      {children}
    </FileSystemContext.Provider>
  );
};

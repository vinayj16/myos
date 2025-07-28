import React, { createContext, useContext, useState, useCallback } from 'react';

export interface Workspace {
  id: number;
  name: string;
}

interface WorkspaceContextType {
  workspaces: Workspace[];
  currentWorkspace: number;
  addWorkspace: () => void;
  switchWorkspace: (id: number) => void;
}

const WorkspaceContext = createContext<WorkspaceContextType | undefined>(undefined);

export const useWorkspaces = () => {
  const context = useContext(WorkspaceContext);
  if (!context) {
    throw new Error('useWorkspaces must be used within a WorkspaceProvider');
  }
  return context;
};

export const WorkspaceProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const [workspaces, setWorkspaces] = useState<Workspace[]>([{ id: 1, name: 'Desktop 1' }]);
  const [currentWorkspace, setCurrentWorkspace] = useState(1);

  const addWorkspace = useCallback(() => {
    const newId = Date.now();
    setWorkspaces(prev => [...prev, { id: newId, name: `Desktop ${prev.length + 1}` }]);
    setCurrentWorkspace(newId);
  }, []);

  const switchWorkspace = useCallback((id: number) => {
    setCurrentWorkspace(id);
  }, []);

  return (
    <WorkspaceContext.Provider value={{ workspaces, currentWorkspace, addWorkspace, switchWorkspace }}>
      {children}
    </WorkspaceContext.Provider>
  );
};

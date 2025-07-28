import React, { createContext, useContext, useState, useCallback, useMemo } from 'react';
import { useWorkspaces } from './WorkspaceContext';

export type SnapPosition = 'left' | 'right' | 'top-left' | 'top-right' | 'bottom-left' | 'bottom-right' | null;

export interface Window {
  id: string;
  title: string;
  component: string;
  workspaceId: number;
  icon?: React.ReactNode;
  isMaximized: boolean;
  isMinimized: boolean;
  position: { x: number; y: number };
  size: { width: number; height: number };
  prevSize?: { width: number; height: number };
  prevPosition?: { x: number; y: number };
  zIndex: number;
}

interface DesktopContextType {
  windows: Window[];
  currentWorkspaceWindows: Window[];
  activeWindowId: string | null;
  nextZIndex: number;
  contextMenuWindowId: string | null;
  contextMenuPos: { x: number, y: number };
  openWindow: (windowConfig: Omit<Window, 'id' | 'zIndex' | 'isMinimized' | 'workspaceId'>) => void;
  closeWindow: (id: string) => void;
  minimizeWindow: (id: string) => void;
  toggleMaximizeWindow: (id: string) => void;
  bringToFront: (id: string) => void;
  updateWindowPosition: (id: string, position: { x: number; y: number }) => void;
  updateWindowSize: (id: string, size: { width: number; height: number }) => void;
  toggleMinimizeAllWindows: () => void;
  snapWindow: (id: string, edge: SnapPosition) => void;
  moveWindowToWorkspace: (windowId: string, workspaceId: number) => void;
  showContextMenu: (event: React.MouseEvent | KeyboardEvent, windowId?: string) => void;
  hideContextMenu: () => void;
}

const DesktopContext = createContext<DesktopContextType | undefined>(undefined);

export const useDesktop = () => {
  const context = useContext(DesktopContext);
  if (!context) {
    throw new Error('useDesktop must be used within a DesktopProvider');
  }
  return context;
};

export const DesktopProvider: React.FC<{ children: React.ReactNode }> = ({ children }) => {
  const { currentWorkspace } = useWorkspaces();
  const [windows, setWindows] = useState<Window[]>([]);
  const [activeWindowId, setActiveWindowId] = useState<string | null>(null);
  const [nextZIndex, setNextZIndex] = useState(1000);
  const [contextMenuWindowId, setContextMenuWindowId] = useState<string | null>(null);
  const [contextMenuPos, setContextMenuPos] = useState({ x: 0, y: 0 });

  const currentWorkspaceWindows = useMemo(() => {
    return windows.filter(w => w.workspaceId === currentWorkspace);
  }, [windows, currentWorkspace]);

  const bringToFront = useCallback((id: string) => {
    setWindows(prev => prev.map(w => 
      w.id === id ? { ...w, isMinimized: false, zIndex: nextZIndex } : w
    ));
    setActiveWindowId(id);
    setNextZIndex(prev => prev + 1);
  }, [nextZIndex]);

  const openWindow = useCallback((windowConfig: Omit<Window, 'id' | 'zIndex' | 'isMinimized' | 'workspaceId'>) => {
    const existingWindow = windows.find(w => w.title === windowConfig.title && w.workspaceId === currentWorkspace);
    if (existingWindow) {
      bringToFront(existingWindow.id);
      return;
    }

    const id = `window-${Date.now()}-${Math.random().toString(36).substr(2, 9)}`;
    const newWindow: Window = {
      ...windowConfig,
      id,
      workspaceId: currentWorkspace,
      isMinimized: false,
      zIndex: nextZIndex,
    };

    setWindows(prev => [...prev, newWindow]);
    setActiveWindowId(id);
    setNextZIndex(prev => prev + 1);
  }, [nextZIndex, windows, bringToFront, currentWorkspace]);

  const closeWindow = useCallback((id: string) => {
    setWindows(prev => prev.filter(w => w.id !== id));
    if (activeWindowId === id) {
      const remainingWindows = windows.filter(w => w.id !== id && w.workspaceId === currentWorkspace);
      const nextActiveWindow = remainingWindows.sort((a,b) => b.zIndex - a.zIndex)[0];
      setActiveWindowId(nextActiveWindow ? nextActiveWindow.id : null);
    }
  }, [activeWindowId, windows, currentWorkspace]);

  const minimizeWindow = useCallback((id: string) => {
    setWindows(prev => prev.map(w => 
      w.id === id ? { ...w, isMinimized: true } : w
    ));
    if (activeWindowId === id) {
      setActiveWindowId(null);
    }
  }, [activeWindowId]);

  const toggleMaximizeWindow = useCallback((id: string) => {
    setWindows(prev => prev.map(w => {
      if (w.id === id) {
        if (w.isMaximized) {
          return { ...w, isMaximized: false, size: w.prevSize || w.size, position: w.prevPosition || w.position };
        } else {
          return { ...w, isMaximized: true, prevSize: w.size, prevPosition: w.position };
        }
      }
      return w;
    }));
    bringToFront(id);
  }, [bringToFront]);
  
  const toggleMinimizeAllWindows = useCallback(() => {
    const allMinimized = currentWorkspaceWindows.every(w => w.isMinimized);
    setWindows(prev => prev.map(w => w.workspaceId === currentWorkspace ? { ...w, isMinimized: !allMinimized } : w));
    if (!allMinimized) setActiveWindowId(null);
  }, [currentWorkspaceWindows, currentWorkspace]);

  const snapWindow = useCallback((id: string, edge: SnapPosition) => {
    const taskbarHeight = 48;
    const { innerWidth: vw, innerHeight: vh } = window;
    const height = vh - taskbarHeight;
    let newPos = { x: 0, y: 0 }, newSize = { width: 0, height: 0 };
    switch (edge) {
      case 'left': newPos = { x: 0, y: 0 }; newSize = { width: vw / 2, height }; break;
      case 'right': newPos = { x: vw / 2, y: 0 }; newSize = { width: vw / 2, height }; break;
      default: return;
    }
    setWindows(prev => prev.map(w => w.id === id ? { ...w, position: newPos, size: newSize, isMaximized: false } : w));
  }, []);

  const updateWindowPosition = useCallback((id: string, position: { x: number; y: number }) => {
    setWindows(prev => prev.map(w => w.id === id ? { ...w, position } : w));
  }, []);

  const updateWindowSize = useCallback((id: string, size: { width: number; height: number }) => {
    setWindows(prev => prev.map(w => w.id === id ? { ...w, size } : w));
  }, []);

  const moveWindowToWorkspace = useCallback((windowId: string, workspaceId: number) => {
    setWindows(prev => prev.map(w => w.id === windowId ? { ...w, workspaceId } : w));
  }, []);

  const hideContextMenu = useCallback(() => {
    setContextMenuWindowId(null);
  }, []);

  const showContextMenu = useCallback((event: React.MouseEvent | KeyboardEvent, windowId?: string) => {
    event.preventDefault();
    const targetId = windowId || activeWindowId;
    if (!targetId) return;
    
    const pos = 'clientX' in event ? { x: event.clientX, y: event.clientY } : { x: 100, y: 100 }; // Fallback for keyboard
    setContextMenuPos(pos);
    setContextMenuWindowId(targetId);
    
    document.addEventListener('click', hideContextMenu, { once: true });
  }, [activeWindowId, hideContextMenu]);

  return (
    <DesktopContext.Provider value={{
      windows, currentWorkspaceWindows, activeWindowId, nextZIndex, contextMenuWindowId, contextMenuPos,
      openWindow, closeWindow, minimizeWindow, toggleMaximizeWindow, bringToFront,
      updateWindowPosition, updateWindowSize, toggleMinimizeAllWindows, snapWindow, moveWindowToWorkspace,
      showContextMenu, hideContextMenu
    }}>
      {children}
    </DesktopContext.Provider>
  );
};

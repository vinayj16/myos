import React from 'react';
import { motion } from 'framer-motion';
import { useDesktop } from '../contexts/DesktopContext';
import { Minimize2, Maximize2, X, Move, ArrowRight, Copy } from 'lucide-react';
import { useWorkspaces } from '../contexts/WorkspaceContext';

interface WindowContextMenuProps {
  windowId: string;
  position: { x: number; y: number };
}

const WindowContextMenu: React.FC<WindowContextMenuProps> = ({ windowId, position }) => {
  const { closeWindow, minimizeWindow, toggleMaximizeWindow, moveWindowToWorkspace, hideContextMenu } = useDesktop();
  const { workspaces } = useWorkspaces();

  const menuItems = [
    { label: 'Minimize', icon: Minimize2, action: () => minimizeWindow(windowId) },
    { label: 'Maximize', icon: Maximize2, action: () => toggleMaximizeWindow(windowId) },
    { label: 'Close', icon: X, action: () => closeWindow(windowId), isDestructive: true },
    { type: 'divider' },
    {
      label: 'Move to Workspace',
      icon: Move,
      subItems: workspaces.map(ws => ({
        label: ws.name,
        icon: ArrowRight,
        action: () => moveWindowToWorkspace(windowId, ws.id),
      })),
    },
  ];

  return (
    <motion.div
      initial={{ opacity: 0, scale: 0.9 }}
      animate={{ opacity: 1, scale: 1 }}
      exit={{ opacity: 0, scale: 0.9 }}
      transition={{ duration: 0.1 }}
      className="fixed z-[9999] bg-white/80 dark:bg-gray-800/80 backdrop-blur-md rounded-lg shadow-2xl border border-gray-300 dark:border-gray-700 p-1 w-56"
      style={{ top: position.y, left: position.x }}
      onMouseLeave={hideContextMenu}
    >
      {menuItems.map((item, index) => {
        if (item.type === 'divider') {
          return <div key={index} className="h-px bg-gray-300 dark:bg-gray-600 my-1" />;
        }
        return (
          <div key={item.label} className="relative group">
            <button
              onClick={item.action}
              className={`w-full flex items-center px-3 py-2 text-sm rounded transition-colors ${
                item.isDestructive
                  ? 'text-red-600 dark:text-red-400 hover:bg-red-500/10'
                  : 'text-gray-800 dark:text-gray-200 hover:bg-black/5 dark:hover:bg-white/10'
              }`}
            >
              <item.icon size={16} className="mr-3" />
              {item.label}
              {item.subItems && <ArrowRight size={16} className="ml-auto" />}
            </button>
            {item.subItems && (
              <div className="absolute left-full top-0 ml-1 hidden group-hover:block bg-white/80 dark:bg-gray-800/80 backdrop-blur-md rounded-lg shadow-2xl border border-gray-300 dark:border-gray-700 p-1 w-56">
                {item.subItems.map(subItem => (
                  <button key={subItem.label} onClick={subItem.action} className="w-full flex items-center px-3 py-2 text-sm rounded transition-colors text-gray-800 dark:text-gray-200 hover:bg-black/5 dark:hover:bg-white/10">
                    <subItem.icon size={16} className="mr-3" />
                    {subItem.label}
                  </button>
                ))}
              </div>
            )}
          </div>
        );
      })}
    </motion.div>
  );
};

export default WindowContextMenu;

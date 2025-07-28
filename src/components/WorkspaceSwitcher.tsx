import React from 'react';
import { motion } from 'framer-motion';
import { useWorkspaces } from '../contexts/WorkspaceContext';
import { useDesktop } from '../contexts/DesktopContext';
import { Plus } from 'lucide-react';

interface WorkspaceSwitcherProps {
  onClose: () => void;
}

const WorkspaceSwitcher: React.FC<WorkspaceSwitcherProps> = ({ onClose }) => {
  const { workspaces, currentWorkspace, addWorkspace, switchWorkspace } = useWorkspaces();
  const { windows, moveWindowToWorkspace } = useDesktop();

  const handleWorkspaceClick = (id: number) => {
    switchWorkspace(id);
    onClose();
  };

  const onDragEnd = (event: MouseEvent | TouchEvent | PointerEvent, info: any, windowId: string) => {
    const dropTarget = (event.target as HTMLElement).closest('.workspace-drop-target');
    if (dropTarget) {
      const workspaceId = Number(dropTarget.getAttribute('data-workspace-id'));
      moveWindowToWorkspace(windowId, workspaceId);
    }
  };

  return (
    <motion.div
      initial={{ opacity: 0 }}
      animate={{ opacity: 1 }}
      exit={{ opacity: 0 }}
      className="fixed inset-0 bg-black/70 backdrop-blur-xl z-[9997] flex flex-col"
      onClick={onClose}
    >
      <div className="flex-grow flex flex-col items-center justify-center p-8 overflow-hidden" onClick={e => e.stopPropagation()}>
        <div className="w-full h-full grid grid-rows-2 grid-cols-2 gap-8">
          {workspaces.map(ws => (
            <div
              key={ws.id}
              data-workspace-id={ws.id}
              className={`workspace-drop-target rounded-lg p-4 transition-colors ${currentWorkspace === ws.id ? 'bg-white/20' : 'bg-white/5 hover:bg-white/10'}`}
              onClick={() => handleWorkspaceClick(ws.id)}
            >
              <h3 className="text-white font-bold mb-2">{ws.name}</h3>
              <div className="relative w-full h-full border-2 border-dashed border-white/20 rounded-md">
                {windows.filter(w => w.workspaceId === ws.id).map(w => (
                  <motion.div
                    key={w.id}
                    layoutId={w.id}
                    drag
                    onDragEnd={(e, i) => onDragEnd(e, i, w.id)}
                    className="absolute bg-blue-500/50 rounded-sm text-white text-xs p-1"
                    style={{
                      width: `${w.size.width / 10}px`,
                      height: `${w.size.height / 10}px`,
                      top: `${w.position.y / 10}px`,
                      left: `${w.position.x / 10}px`,
                    }}
                  >
                    {w.title}
                  </motion.div>
                ))}
              </div>
            </div>
          ))}
        </div>
      </div>
      <div className="flex-shrink-0 p-4">
        <div className="flex justify-center items-center gap-4">
          {workspaces.map(ws => (
            <button key={ws.id} onClick={() => switchWorkspace(ws.id)} className={`w-24 h-16 rounded transition-colors ${currentWorkspace === ws.id ? 'bg-blue-500' : 'bg-white/20 hover:bg-white/30'}`}>
              <span className="text-white text-sm">{ws.name}</span>
            </button>
          ))}
          <button onClick={addWorkspace} className="w-16 h-16 rounded bg-white/20 hover:bg-white/30 flex items-center justify-center">
            <Plus className="h-8 w-8 text-white" />
          </button>
        </div>
      </div>
    </motion.div>
  );
};

export default WorkspaceSwitcher;

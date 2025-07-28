import React, { useState } from 'react';
import { motion, useDragControls } from 'framer-motion';
import { Minimize2, Maximize2, X } from 'lucide-react';
import { useDesktop, Window as WindowType, SnapPosition } from '../contexts/DesktopContext';
import Terminal from './applications/Terminal';
import FileManager from './applications/FileManager';
import StartMenu from './applications/StartMenu';
import SystemMonitor from './applications/SystemMonitor';
import TextEditor from './applications/TextEditor';
import AppCalculator from './applications/Calculator';
import SettingsApp from './applications/Settings';
import WebBrowser from './applications/WebBrowser';
import ImageViewer from './applications/ImageViewer';
import Calendar from './applications/Calendar';
import PackageManager from './applications/PackageManager';

interface WindowProps {
  window: WindowType;
}

const Window: React.FC<WindowProps> = ({ window }) => {
  const { closeWindow, minimizeWindow, toggleMaximizeWindow, bringToFront, updateWindowPosition, snapWindow, activeWindowId, showContextMenu } = useDesktop();
  const dragControls = useDragControls();
  const [snapPreview, setSnapPreview] = useState<SnapPosition>(null);

  const handleHeaderMouseDown = (e: React.MouseEvent) => {
    bringToFront(window.id);
    if (!window.isMaximized) {
      dragControls.start(e);
    }
  };

  const renderWindowContent = () => {
    switch (window.component) {
      case 'Terminal': return <Terminal />;
      case 'FileManager': return <FileManager />;
      case 'StartMenu': return <StartMenu />;
      case 'SystemMonitor': return <SystemMonitor />;
      case 'TextEditor': return <TextEditor />;
      case 'Calculator': return <AppCalculator />;
      case 'Settings': return <SettingsApp />;
      case 'WebBrowser': return <WebBrowser />;
      case 'ImageViewer': return <ImageViewer />;
      case 'Calendar': return <Calendar />;
      case 'PackageManager': return <PackageManager />;
      default: return <div className="p-4 text-gray-600">Component {window.component} not found.</div>;
    }
  };

  if (window.isMinimized) return null;

  const handleDrag = (event: MouseEvent | TouchEvent | PointerEvent) => {
    const { clientX: x } = event as MouseEvent;
    const edgeThreshold = 5;
    if (x < edgeThreshold) setSnapPreview('left');
    else if (x > window.innerWidth - edgeThreshold) setSnapPreview('right');
    else setSnapPreview(null);
  };

  const handleDragEnd = (event: MouseEvent | TouchEvent | PointerEvent, info: any) => {
    if (snapPreview) {
      snapWindow(window.id, snapPreview);
      setSnapPreview(null);
    } else {
      updateWindowPosition(window.id, {
        x: Math.max(0, window.position.x + info.offset.x),
        y: Math.max(0, window.position.y + info.offset.y),
      });
    }
  };

  const windowVariants = {
    hidden: { opacity: 0, scale: 0.9, transition: { type: 'spring', stiffness: 500, damping: 30 } },
    visible: { opacity: 1, scale: 1, transition: { type: 'spring', stiffness: 300, damping: 25 } },
    exit: { opacity: 0, scale: 0.9, transition: { duration: 0.15 } },
  };

  const isActive = activeWindowId === window.id;

  return (
    <>
      <motion.div
        drag dragListener={false} dragControls={dragControls} dragMomentum={false}
        onDrag={handleDrag} onDragEnd={handleDragEnd}
        variants={windowVariants} initial="hidden" animate="visible" exit="exit"
        layoutId={window.id}
        className={`absolute pointer-events-auto window flex flex-col ${isActive ? 'shadow-blue-500/50' : ''}`}
        style={{
          zIndex: window.zIndex,
          top: window.position.y, left: window.position.x,
          width: window.size.width, height: window.size.height,
          ...(window.isMaximized && { top: 0, left: 0, width: '100vw', height: 'calc(100vh - 48px)', borderRadius: 0 }),
        }}
        onMouseDown={() => bringToFront(window.id)}
        onContextMenu={(e) => showContextMenu(e, window.id)}
      >
        <div onPointerDown={handleHeaderMouseDown} onDoubleClick={() => toggleMaximizeWindow(window.id)} className={`window-header cursor-grab active:cursor-grabbing select-none ${isActive ? 'bg-gray-200 dark:bg-gray-600' : ''}`}>
          <span className="font-medium text-gray-800 dark:text-gray-200">{window.title}</span>
          <div className="window-controls">
            <button onClick={() => minimizeWindow(window.id)} className="window-control bg-yellow-400 hover:bg-yellow-500" />
            <button onClick={() => toggleMaximizeWindow(window.id)} className="window-control bg-green-400 hover:bg-green-500" />
            <button onClick={() => closeWindow(window.id)} className="window-control bg-red-400 hover:bg-red-500" />
          </div>
        </div>
        <div className="window-content flex-1 overflow-hidden">{renderWindowContent()}</div>
      </motion.div>
      {snapPreview && <div className={`snap-overlay ${snapPreview === 'left' ? 'left-0' : 'right-0'} top-0 w-1/2 h-[calc(100vh-48px)]`} />}
    </>
  );
};

export default Window;

import React, { useState } from 'react';
import { AnimatePresence } from 'framer-motion';
import Wallpaper from './Wallpaper';
import Taskbar from './Taskbar';
import WindowManager from './WindowManager';
import DesktopIcons from './DesktopIcons';
import AIAssistant from './AIAssistant';
import WindowSwitcher from './WindowSwitcher';
import RunDialog from './RunDialog';
import NotificationCenter from './NotificationCenter';
import WorkspaceSwitcher from './WorkspaceSwitcher';
import HotCorners from './HotCorners';
import WindowContextMenu from './WindowContextMenu';
import { useDesktop } from '../contexts/DesktopContext';
import { useHotkeys } from '../hooks/useHotkeys';

interface DesktopProps {
  onLogout: () => void;
  onLock: () => void;
}

const Desktop: React.FC<DesktopProps> = ({ onLogout, onLock }) => {
  const [showAIAssistant, setShowAIAssistant] = useState(false);
  const [showWindowSwitcher, setShowWindowSwitcher] = useState(false);
  const [showRunDialog, setShowRunDialog] = useState(false);
  const [showNotificationCenter, setShowNotificationCenter] = useState(false);
  const [showWorkspaceSwitcher, setShowWorkspaceSwitcher] = useState(false);

  const { toggleMinimizeAllWindows, openWindow, closeWindow, activeWindowId, showContextMenu, contextMenuPos, contextMenuWindowId } = useDesktop();

  const handleScreenshot = () => {
    const event = new Event('request-screenshot');
    window.dispatchEvent(event);
  };

  const hotkeys = [
    { key: 'a', ctrl: true, alt: true, callback: () => setShowAIAssistant(p => !p) },
    { key: 'd', meta: true, callback: toggleMinimizeAllWindows },
    { key: 'l', meta: true, callback: onLock },
    { key: 'p', meta: true, callback: () => openWindow({ title: 'Settings', component: 'Settings', position: { x: 250, y: 150 }, size: { width: 500, height: 400 } }) },
    { key: 'r', meta: true, callback: () => setShowRunDialog(p => !p) },
    { key: 'e', meta: true, callback: () => openWindow({ title: 'File Manager', component: 'FileManager', position: { x: 150, y: 50 }, size: { width: 700, height: 500 } }) },
    { key: 'Tab', alt: true, callback: () => setShowWindowSwitcher(true) },
    { key: 'Tab', meta: true, callback: () => setShowWorkspaceSwitcher(p => !p) },
    { key: 's', meta: true, shift: true, callback: handleScreenshot },
    { key: ' ', alt: true, callback: (e: KeyboardEvent) => showContextMenu(e) },
    { key: 'F4', alt: true, callback: () => activeWindowId && closeWindow(activeWindowId) },
  ];
  useHotkeys(hotkeys);

  return (
    <div className="w-full h-full relative overflow-hidden">
      <Wallpaper />
      <DesktopIcons />
      <WindowManager />
      <HotCorners onShowWorkspaces={() => setShowWorkspaceSwitcher(true)} onShowDesktop={toggleMinimizeAllWindows} />
      
      <Taskbar 
        onAIAssistantToggle={() => setShowAIAssistant(p => !p)}
        onShowDesktop={toggleMinimizeAllWindows}
        onNotificationToggle={() => setShowNotificationCenter(p => !p)}
        onWorkspaceToggle={() => setShowWorkspaceSwitcher(p => !p)}
      />

      <AnimatePresence>
        {showAIAssistant && <AIAssistant onClose={() => setShowAIAssistant(false)} />}
        {showWindowSwitcher && <WindowSwitcher onClose={() => setShowWindowSwitcher(false)} />}
        {showRunDialog && <RunDialog onClose={() => setShowRunDialog(false)} />}
        {showNotificationCenter && <NotificationCenter onClose={() => setShowNotificationCenter(false)} />}
        {showWorkspaceSwitcher && <WorkspaceSwitcher onClose={() => setShowWorkspaceSwitcher(false)} />}
        {contextMenuWindowId && <WindowContextMenu windowId={contextMenuWindowId} position={contextMenuPos} />}
      </AnimatePresence>
    </div>
  );
};

export default Desktop;

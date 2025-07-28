import { useEffect } from 'react';

type Hotkey = {
  key: string;
  ctrl?: boolean;
  alt?: boolean;
  shift?: boolean;
  meta?: boolean; // For Windows/Command key
  callback: (event: KeyboardEvent) => void;
};

export const useHotkeys = (hotkeys: Hotkey[]) => {
  useEffect(() => {
    const handleKeyDown = (event: KeyboardEvent) => {
      for (const hotkey of hotkeys) {
        if (
          event.key.toLowerCase() === hotkey.key.toLowerCase() &&
          (hotkey.ctrl === undefined || event.ctrlKey === hotkey.ctrl) &&
          (hotkey.alt === undefined || event.altKey === hotkey.alt) &&
          (hotkey.shift === undefined || event.shiftKey === hotkey.shift) &&
          (hotkey.meta === undefined || event.metaKey === hotkey.meta)
        ) {
          event.preventDefault();
          hotkey.callback(event);
        }
      }
    };

    window.addEventListener('keydown', handleKeyDown);
    return () => {
      window.removeEventListener('keydown', handleKeyDown);
    };
  }, [hotkeys]);
};

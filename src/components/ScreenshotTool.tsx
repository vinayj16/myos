import React, { useEffect, useState } from 'react';
import html2canvas from 'html2canvas';
import { motion } from 'framer-motion';
import { Camera, Download, X } from 'lucide-react';

interface ScreenshotToolProps {
  onDone: () => void;
}

const ScreenshotTool: React.FC<ScreenshotToolProps> = ({ onDone }) => {
  const [screenshot, setScreenshot] = useState<string | null>(null);
  const [isCapturing, setIsCapturing] = useState(true);

  useEffect(() => {
    const capture = async () => {
      const desktopElement = document.getElementById('desktop-container');
      if (desktopElement) {
        const canvas = await html2canvas(desktopElement, { useCORS: true });
        setScreenshot(canvas.toDataURL('image/png'));
        setIsCapturing(false);
      } else {
        onDone();
      }
    };
    capture();
  }, [onDone]);

  const handleDownload = () => {
    if (screenshot) {
      const link = document.createElement('a');
      link.href = screenshot;
      link.download = `MyOS-Screenshot-${Date.now()}.png`;
      document.body.appendChild(link);
      link.click();
      document.body.removeChild(link);
      onDone();
    }
  };

  return (
    <motion.div
      initial={{ opacity: 0 }}
      animate={{ opacity: 1 }}
      exit={{ opacity: 0 }}
      className="fixed inset-0 bg-black/70 backdrop-blur-sm z-[10000] flex items-center justify-center"
    >
      {isCapturing && (
        <div className="text-white flex items-center">
          <Camera className="animate-pulse h-8 w-8 mr-4" />
          <p className="text-2xl">Capturing screen...</p>
        </div>
      )}
      {screenshot && (
        <motion.div
          initial={{ scale: 0.8, opacity: 0 }}
          animate={{ scale: 1, opacity: 1 }}
          className="bg-gray-800 p-4 rounded-lg shadow-2xl border border-gray-600"
        >
          <h2 className="text-white text-lg font-semibold mb-4">Screenshot Captured</h2>
          <img src={screenshot} alt="Screenshot" className="max-w-xl max-h-[60vh] rounded border border-gray-500" />
          <div className="flex justify-end space-x-2 mt-4">
            <button onClick={handleDownload} className="flex items-center px-4 py-2 bg-blue-500 text-white rounded-lg hover:bg-blue-600">
              <Download size={16} className="mr-2" /> Download
            </button>
            <button onClick={onDone} className="flex items-center px-4 py-2 bg-gray-600 text-white rounded-lg hover:bg-gray-500">
              <X size={16} className="mr-2" /> Close
            </button>
          </div>
        </motion.div>
      )}
    </motion.div>
  );
};

export default ScreenshotTool;

import React from 'react';
import { Image, ZoomIn, ZoomOut, RotateCw } from 'lucide-react';

const ImageViewer: React.FC = () => {
  return (
    <div className="h-full flex flex-col bg-gray-800 text-white">
      <div className="flex-grow flex items-center justify-center p-4">
        <div className="text-center">
          <Image size={128} className="text-gray-500 mx-auto" />
          <p className="mt-4 text-gray-400">No image loaded. This is a placeholder.</p>
        </div>
      </div>
      <div className="flex-shrink-0 bg-gray-900/50 p-2 flex justify-center items-center space-x-4">
        <button className="p-2 hover:bg-gray-700 rounded-full"><ZoomIn /></button>
        <button className="p-2 hover:bg-gray-700 rounded-full"><ZoomOut /></button>
        <button className="p-2 hover:bg-gray-700 rounded-full"><RotateCw /></button>
      </div>
    </div>
  );
};

export default ImageViewer;

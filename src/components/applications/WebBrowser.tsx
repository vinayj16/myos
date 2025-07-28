import React, { useState } from 'react';
import { motion } from 'framer-motion';
import { Globe, RefreshCw, Home, ArrowLeft, ArrowRight, Lock } from 'lucide-react';

const WebBrowser: React.FC = () => {
  const [url, setUrl] = useState('https://myos.com/welcome');
  const [history, setHistory] = useState(['https://myos.com/welcome']);
  const [historyIndex, setHistoryIndex] = useState(0);

  const navigate = (newUrl: string) => {
    const updatedHistory = history.slice(0, historyIndex + 1);
    updatedHistory.push(newUrl);
    setHistory(updatedHistory);
    setHistoryIndex(updatedHistory.length - 1);
    setUrl(newUrl);
  };

  const goBack = () => {
    if (historyIndex > 0) {
      const newIndex = historyIndex - 1;
      setHistoryIndex(newIndex);
      setUrl(history[newIndex]);
    }
  };

  const goForward = () => {
    if (historyIndex < history.length - 1) {
      const newIndex = historyIndex + 1;
      setHistoryIndex(newIndex);
      setUrl(history[newIndex]);
    }
  };

  return (
    <div className="h-full flex flex-col bg-gray-100 dark:bg-gray-800">
      <div className="flex items-center p-2 bg-gray-200 dark:bg-gray-700">
        <button onClick={goBack} disabled={historyIndex === 0} className="p-1 rounded hover:bg-gray-300 dark:hover:bg-gray-600 disabled:opacity-50"><ArrowLeft size={20} /></button>
        <button onClick={goForward} disabled={historyIndex === history.length - 1} className="p-1 rounded hover:bg-gray-300 dark:hover:bg-gray-600 disabled:opacity-50"><ArrowRight size={20} /></button>
        <button className="p-1 rounded hover:bg-gray-300 dark:hover:bg-gray-600"><RefreshCw size={20} /></button>
        <div className="flex-grow mx-2 flex items-center bg-white dark:bg-gray-800 rounded-full px-3 py-1">
          <Lock size={14} className="text-green-600 mr-2" />
          <input type="text" value={url} readOnly className="w-full bg-transparent outline-none text-sm" />
        </div>
      </div>
      <div className="flex-grow p-8 bg-white dark:bg-gray-900 text-center flex flex-col items-center justify-center">
        <Globe size={64} className="text-blue-500 mb-4" />
        <h1 className="text-3xl font-bold mb-2">Welcome to MyBrowser</h1>
        <p className="text-lg text-gray-600 dark:text-gray-400">This is a simulated web browser environment.</p>
        <p>Current page: <code className="bg-gray-200 dark:bg-gray-700 p-1 rounded">{url}</code></p>
      </div>
    </div>
  );
};

export default WebBrowser;

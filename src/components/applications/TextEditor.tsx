import React, { useState } from 'react';
import { motion } from 'framer-motion';
import { Save, FilePlus, FolderOpen, Bold, Italic, Underline } from 'lucide-react';

const TextEditor: React.FC = () => {
  const [text, setText] = useState('Welcome to MyOS Text Editor!\n\nStart typing here...');
  const [fileName, setFileName] = useState('Untitled.txt');

  return (
    <div className="h-full flex flex-col bg-white dark:bg-gray-900 text-gray-900 dark:text-gray-100">
      {/* Toolbar */}
      <div className="flex items-center p-2 border-b border-gray-200 dark:border-gray-700 space-x-2">
        <button className="p-2 hover:bg-gray-100 dark:hover:bg-gray-800 rounded"><FilePlus size={18} /></button>
        <button className="p-2 hover:bg-gray-100 dark:hover:bg-gray-800 rounded"><FolderOpen size={18} /></button>
        <button className="p-2 hover:bg-gray-100 dark:hover:bg-gray-800 rounded"><Save size={18} /></button>
        <div className="h-6 w-px bg-gray-300 dark:bg-gray-600" />
        <button className="p-2 hover:bg-gray-100 dark:hover:bg-gray-800 rounded"><Bold size={18} /></button>
        <button className="p-2 hover:bg-gray-100 dark:hover:bg-gray-800 rounded"><Italic size={18} /></button>
        <button className="p-2 hover:bg-gray-100 dark:hover:bg-gray-800 rounded"><Underline size={18} /></button>
      </div>

      {/* File Name */}
      <div className="p-2 border-b border-gray-200 dark:border-gray-700">
        <input 
          type="text"
          value={fileName}
          onChange={(e) => setFileName(e.target.value)}
          className="w-full bg-transparent focus:outline-none text-sm"
        />
      </div>

      {/* Text Area */}
      <textarea
        value={text}
        onChange={(e) => setText(e.target.value)}
        className="flex-1 w-full p-4 bg-gray-50 dark:bg-gray-800 resize-none focus:outline-none font-mono text-sm"
        spellCheck="false"
      />

      {/* Status Bar */}
      <div className="p-1 border-t border-gray-200 dark:border-gray-700 text-xs text-gray-500 dark:text-gray-400">
        <p>Lines: {text.split('\n').length}, Characters: {text.length}</p>
      </div>
    </div>
  );
};

export default TextEditor;

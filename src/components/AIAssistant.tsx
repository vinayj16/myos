import React, { useState, useRef, useEffect } from 'react';
import { motion } from 'framer-motion';
import { Bot, Send, Mic, X, MicOff } from 'lucide-react';
import { useDesktop } from '../contexts/DesktopContext';
import { useNotifications } from '../contexts/NotificationContext';
import { useSpeechRecognition } from 'react-speech-kit';

interface Message {
  text: string;
  sender: 'user' | 'ai';
}

interface AIAssistantProps {
  onClose: () => void;
}

const AIAssistant: React.FC<AIAssistantProps> = ({ onClose }) => {
  const [messages, setMessages] = useState<Message[]>([
    { sender: 'ai', text: 'Hello! I am MyOS AI. How can I assist you? Try saying "Open Terminal".' },
  ]);
  const [inputText, setInputText] = useState('');
  const { openWindow } = useDesktop();
  const { addNotification } = useNotifications();
  const messagesEndRef = useRef<HTMLDivElement>(null);

  const { listen, listening, stop, supported } = useSpeechRecognition({
    onResult: (result) => {
      setInputText(result);
    },
  });

  const handleSendMessage = () => {
    if (!inputText.trim()) return;

    const userMessage: Message = { text: inputText, sender: 'user' };
    setMessages(prev => [...prev, userMessage]);
    
    setTimeout(() => {
      let aiResponseText = "I'm not sure how to handle that. Try asking me to open an application like 'open terminal'.";
      const command = inputText.toLowerCase();

      if (command.includes('open') || command.includes('launch')) {
        const apps = ['terminal', 'calculator', 'settings', 'file manager', 'text editor', 'package manager'];
        const appToOpen = apps.find(app => command.includes(app));
        
        if (appToOpen) {
          const title = appToOpen.split(' ').map(w => w.charAt(0).toUpperCase() + w.slice(1)).join(' ');
          const component = title.replace(/\s/g, '');
          openWindow({ title, component, position: { x: 200, y: 200 }, size: { width: 600, height: 400 } });
          aiResponseText = `Opening ${title} for you.`;
        } else {
          aiResponseText = "I can't find that application.";
        }
      } else if (command.includes('notification')) {
        addNotification({ title: 'AI Notification', message: 'Here is a test notification you requested!' });
        aiResponseText = 'I\'ve sent a test notification.';
      }

      const aiMessage: Message = { text: aiResponseText, sender: 'ai' };
      setMessages(prev => [...prev, aiMessage]);
    }, 1000);

    setInputText('');
  };

  useEffect(() => {
    messagesEndRef.current?.scrollIntoView({ behavior: 'smooth' });
  }, [messages]);

  return (
    <motion.div
      initial={{ opacity: 0, scale: 0.9 }}
      animate={{ opacity: 1, scale: 1 }}
      exit={{ opacity: 0, scale: 0.9 }}
      transition={{ duration: 0.2 }}
      className="absolute inset-0 z-50 flex items-center justify-center bg-black/50 backdrop-blur-sm"
      onClick={onClose}
    >
      <div onClick={(e) => e.stopPropagation()} className="w-96 h-[500px] bg-white dark:bg-gray-800 rounded-2xl shadow-2xl border border-gray-200 dark:border-gray-700 flex flex-col overflow-hidden">
        <div className="bg-gradient-to-r from-primary-light to-primary-dark px-4 py-3 flex items-center justify-between">
          <div className="flex items-center"><Bot className="h-5 w-5 text-white mr-2" /><span className="text-white font-semibold">AI Assistant</span></div>
          <button onClick={onClose} className="text-white hover:bg-black/20 p-1 rounded-full"><X className="h-4 w-4" /></button>
        </div>
        <div className="flex-1 overflow-y-auto p-4 space-y-4">
          {messages.map((message, index) => (
            <motion.div key={index} initial={{ opacity: 0, y: 10 }} animate={{ opacity: 1, y: 0 }} className={`flex ${message.sender === 'user' ? 'justify-end' : 'justify-start'}`}>
              <div className={`max-w-xs p-3 rounded-lg ${message.sender === 'user' ? 'bg-blue-500 text-white rounded-br-none' : 'bg-gray-100 dark:bg-gray-700 text-gray-800 dark:text-gray-200 rounded-bl-none'}`}>
                <p className="text-sm">{message.text}</p>
              </div>
            </motion.div>
          ))}
          <div ref={messagesEndRef} />
        </div>
        <div className="p-4 border-t border-gray-200 dark:border-gray-700">
          <div className="flex items-center space-x-2">
            {supported && (
              <button onMouseDown={listen} onMouseUp={stop} className={`p-2 rounded-lg ${listening ? 'bg-red-500 text-white' : 'bg-gray-200 dark:bg-gray-600'}`}>
                {listening ? <MicOff className="h-4 w-4" /> : <Mic className="h-4 w-4" />}
              </button>
            )}
            <input type="text" value={inputText} onChange={(e) => setInputText(e.target.value)} onKeyPress={(e) => e.key === 'Enter' && handleSendMessage()} placeholder="Ask or say something..." className="flex-1 px-3 py-2 border border-gray-300 dark:border-gray-600 rounded-lg focus:outline-none focus:ring-2 focus:ring-blue-500 dark:bg-gray-700 dark:text-white" />
            <button onClick={handleSendMessage} className="p-2 bg-blue-500 hover:bg-blue-600 rounded-lg"><Send className="h-4 w-4 text-white" /></button>
          </div>
        </div>
      </div>
    </motion.div>
  );
};

export default AIAssistant;

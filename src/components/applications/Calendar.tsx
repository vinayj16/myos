import React from 'react';
import { Calendar as CalendarIcon, ChevronLeft, ChevronRight } from 'lucide-react';

const Calendar: React.FC = () => {
  const days = ['Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat'];
  const dates = Array.from({ length: 35 }, (_, i) => i - 3);

  return (
    <div className="h-full flex flex-col bg-white dark:bg-gray-900 text-gray-900 dark:text-gray-100 p-4">
      <div className="flex items-center justify-between mb-4">
        <h2 className="text-xl font-bold">January 2025</h2>
        <div className="flex items-center space-x-2">
          <button className="p-1 rounded hover:bg-gray-100 dark:hover:bg-gray-800"><ChevronLeft /></button>
          <button className="p-1 rounded hover:bg-gray-100 dark:hover:bg-gray-800"><ChevronRight /></button>
        </div>
      </div>
      <div className="grid grid-cols-7 gap-1 text-center font-medium text-gray-500">
        {days.map(day => <div key={day} className="py-2">{day}</div>)}
      </div>
      <div className="grid grid-cols-7 gap-1 flex-grow">
        {dates.map(date => (
          <div key={date} className={`p-2 border border-gray-200 dark:border-gray-700 rounded ${date < 1 || date > 31 ? 'text-gray-400 dark:text-gray-600' : ''} ${date === 26 ? 'bg-blue-500 text-white font-bold' : 'hover:bg-gray-100 dark:hover:bg-gray-800'}`}>
            {date > 0 && date < 32 ? date : ''}
          </div>
        ))}
      </div>
    </div>
  );
};

export default Calendar;

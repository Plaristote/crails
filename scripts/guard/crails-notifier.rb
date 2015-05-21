module Crails
  module Notifier
    def self.notify title, message
      notifier = notifiers.first
      notifiers.first.notify title, message unless notifier.nil?
    end

    def self.notifiers
      [
        NotifySend, KDialog, TerminalNotifier
      ].select {|i| i.is_available? }
    end

    class NotifySend
      def self.is_available?
        `which notify-send 2> /dev/null`
        $?.success?
      end

      def self.notify title, message
        `notify-send "#{message}" -t 2500`
      end
    end

    class KDialog
      def self.is_available?
        `which kdialog 2> /dev/null`
        $?.success?
      end

      def self.notify title, message
        `kdialog --title "#{title}" --passivepopup "#{message}"`
      end
    end

    class TerminalNotifier
      def self.is_available?
        `which terminal-notifier 2> /dev/null`
        $?.success?
      end

      def self.notify title, message
        `terminal-notifier -title "#{title}" -message "#{message}"`
      end
    end
  end
end


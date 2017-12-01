from threading import Lock
from item import ITEM
import csv

class Castle:
    def __init__(self, id, question_id):
        self.id = id
        self.owner_id = None
        self.defence = 0
        self.is_blocked = False
        self.team_attacking = None
        self.block_time = 0
        self.question_id = question_id
        self.gold_delay = 30
        self.lock = Lock()

    def change_def(self, item_name):
        """Change castle's defence
        :param item_name: name of item
        :return result
        """
        result = None

        with self.lock:
            if item_name not in list(ITEM['defence'].keys()):
                result = False
            else:
                self.defence = ITEM['defence'][item_name]
                result = True

        return result

    def change_question(self):
        """Change question's id"""
        self.question_id += 3

    def remove_block(self):
        """Remove castle blocking"""
        with self.lock:
            self.block_time = 0
            self.is_blocked = False

    def set_block(self):
        """Set castle to blocked state"""
        with self.lock:
            self.block_time = 5 * 60
            self.is_blocked = True
        return True

    def is_attack_success(self, item_name):
        """Check whether an attack is sucessed
        :param item_name: name of item
        :return result
        """
        result = None

        with self.lock:
            if self.defence == 0:
                result = True
            elif item_name not in list(ITEM['attack'].keys()):
                result = False
            else:
                try:
                    result = self.defence <= ITEM['attack'][item_name]['value']
                except:
                    result = False
        return result

    def attacked(self, team_id):
        """Set castle to be attacked state
        :param team_id: team's id
        :return result
        """
        result = False

        with self.lock:
            if self.is_blocked:
                result = False
            else:
                self.is_blocked = True
                self.block_time = 5 * 60
                self.team_attacking = team_id

                result = True

        return result

    def change_owner(self, team_id):
        """Change castle's owner
        :param team_id: team's id
        :return result
        """
        result = None

        with self.lock:
            if self.owner_id == team_id:
                result = False
            else:
                self.owner_id = team_id
                self.gold_delay = 30

                result = True

        return result

    def reduce_gold_delay(self):
        """Reduce gold's counter
        :return is_have_resource: whether owner gets gold
        """
        is_have_resource = False

        with self.lock:
            self.gold_delay -= 1

            if self.gold_delay == -1:
                self.gold_delay = 30
                is_have_resource = True

        return is_have_resource

    def reduce_block(self):
        """Reduce castle's blocking time"""
        with self.lock:
            if self.block_time > 0:
                self.block_time -= 1
            else:
                self.is_blocked = False

    @staticmethod
    def get_castles_from_file(file_name='model/castles.csv'):
        """Get castle list from file
        :param file_name: file name
        :return castles: a list of castle
        """
        castles = []
        with open(file_name) as csvfile:
            csvreader = csv.reader(csvfile, delimiter=',', quotechar='"')
            for index, row in enumerate(csvreader):
                castles.append(Castle(int(row[0]), int(row[1])))
        return castles

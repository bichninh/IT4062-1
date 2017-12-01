from threading import Lock
from item import ITEM
import csv


class Team:
    def __init__(self, id, name):
        self.id = id
        self.name = name
        self.members = []
        self.resources = {
            "gold": 100000,
            "iron": 100000,
            "stone": 100000,
            "wood": 100000,
        }
        self.inventory = []
        self.lock = Lock()

    def add_member(self, member):
        """Add member to team
        :param member: member's id
        :return result(boolean)
        """
        if len(self.members) >= 4:
            return False
        self.members.append(member)
        return True

    def add_resource(self, resource_type, amount):
        """Add an amount of resource to team
        :param resource_type: a kind of resource
        :param amount: number of resource
        """
        with self.lock:
            self.resources[resource_type] += amount

    def __reduce_resource(self, resource_type, amount):
        """Reduce an amount of resource
        :notice:: Must use under a lock
        """
        if self.resources[resource_type] >= amount:
            self.resources[resource_type] -= amount

            return True

        return False

    def __add_item(self, item_name):
        """Add item to inventory
        :notice:: Must use under a lock
        """
        if item_name not in self.inventory:
            self.inventory.append(item_name)
            return True
        return False

    def __is_enough(self, price):
        """Is enough resources to purchase
        :notice:: Must use under a lock
        """
        types = price.keys()

        is_enough = True

        for type in types:
            is_enough = (is_enough
                         and self.resources[type] >= price[type])

        return is_enough

    def is_have(self, item_name):
        """Is have an item"""
        # Thread safe
        return item_name in self.inventory

    def buy_item(self, item_type, item_name):
        """Buy an item
        :param item_type: Kind of item
        :param item_name: Item's name
        :return result(Boolean)
        """
        if item_name not in ITEM[item_type].keys():
            return False

        price = ITEM[item_type][item_name]['resources']
        if self.__is_enough(price) is not True:
            return 'not_enough'
        result = False
        with self.lock:
            for type, amount in price.items():
                self.__reduce_resource(type, amount)
            if item_type == 'attack':
                self.inventory = []
                self.inventory.append(item_name)
                result = True
            elif item_type == 'defence':
                result = True
        return result

    def use_item(self, item_name):
        """Use a item
        :param item_name: item's name
        :return result
        """
        result = False

        with self.lock:
            if item_name in self.inventory:
                self.inventory.remove(item_name)

                result = True

        return result

    @staticmethod
    def get_teams_from_file(file_name='model/teams.csv'):
        """Create team list from a CSV file
        :param file_name: file name (Default value='teams.csv')
        :return teams: a list of teams
        """
        teams = []

        with open(file_name) as csvfile:
            csvreader = csv.reader(csvfile, delimiter=',', quotechar='"')

            for index, row in enumerate(csvreader):
                teams.append(Team(int(row[0]), row[1]))

        return teams

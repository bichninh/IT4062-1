import os, sys
# import path

sys.path.append(os.path.abspath('model'))
sys.path.append(os.path.abspath('../model'))

from question import *
from field import *
from castle import *
from team import *
from item import *

class controller:
    def __init__(self, file_name_question='controller/question.csv'):
    	"""Initialize this controller object with arrays of all  entity object.
    	The Teams, Fields, Castles, Questions object are initialized and store in arrays
        
        :Parameters:
         
        """
        self.questions = Question.get_questions(file_name_question)
        self.teams = Team.get_teams_from_file()
        self.castles = Castle.get_castles_from_file()
        self.fields = Field.get_fields_from_file()

    def join_team(self, player, team_id):
        """Process when one play want to join team

        This function checks if the party get full members or not and add the new member
    
        
        :Parameters:
         player : string
                address of player want to join game
         team_id : int
                The id of team to join in.
    
        :Return: True if success. False if not found the team. "max_players" if the team is full
        """
        
        for i in self.teams:
            if i.id == team_id:
                if i.add_member(player):
                    return True
                else:
                    return "max_players"
        return False
    def get_question_by_id(self, question_id):
        """Get content and 4 choices of question which has question_id.

        :Parameters:
          question_id : int
            The id of the question which player want to get content 
        :Return: byte string (content, [choice1, choice2,...])
        """
        for i in self.questions:
            if i.id == question_id:
                return i.get_question()
        return None, None

    def check_question_mine(self,mine_id, resource, team_id):
        """Check that question of field is already answer .

        :Parameters:
          mine_id : int
            id of field
          resource : byte string
            A field has 3 resource with 3 question, choose one of them
          team_id : int
            The id of the team  
        :Return: Boolean 
        """
        for i in self.fields:
            if i.id == mine_id:
                return i.is_solved(resource, team_id)

    def get_questionid_mine(self,mine_id, resource):
        """Get question content of resource in a field.

        :Parameters:
          mine_id : int
            id of field
          resource : byte string
            A field has 3 resource with 3 question, choose one of them
           
        :Return: int (question_id)
        """
        for i in self.fields:
            if i.id == mine_id:
                return i.get_question_id(resource)
        return False
    def check_answer(self, answer, question_id):
        """Check the answer of the question.

        :Parameters:
          answer : byte string
            answer string ('1','2','3','4')
          question_id : int
            The id of the question 
        :Return: Boolean or byte string (if it had error)
        """
        for i in self.questions:
            if i.id == question_id:
                return i.check_answer(answer)
        return "Not_found_question"

    def buy_item(self, team_id, itemname, type):
         """Buy a item.

        :Parameters:
          team_id : int
            The id of the team 
          itemname : byte string
            name the item want to buy  
        :Return: Boolean or byte string (if it had error)
        """
        for team in self.teams:
            if team.id == team_id:
                return team.buy_item(type, itemname)
        return "not_found_team"

    def check_item_attack_team(self, team_id, itemname):
        """Check attack item that the new is better than the old one
        :Parameters:
          castle_id : int
            id of castle
          itemname : byte string
            name the item want to check
        :Return: True if the new one is better or "you_had_best" if the old the still good
        """
        for team in self.teams:
            if team.id == team_id:
                if len(team.inventory) == 0:
                    return True
                else:
                    if itemname == team.inventory[0]:
                        return "had_it"
                    if ITEM['attack'][itemname]['value'] > ITEM['attack'][team.inventory[0]]['value']:
                        return True
                    else:
                        return "you_had_best"

    def buy_attack(self, team_id, itemname):
        """Get content and 4 choices of question of that resource.

        :Parameters:
          mine_id : int
            id of field
          resource : byte string
            A field has 3 resource with 3 question, choose one of them
        :Return: byte string (content, [choice1, choice2,...])
        """
        for team in self.teams:
            if team.id == team_id:
                result = self.check_item_attack_team(team_id, itemname)
                if result != True:
                    return result
                else:
                    return self.buy_item(team_id, itemname, 'attack')

    def check_item_defence(self, castle_id, itemname):
        """Check defence item that the new is better than the old one
        :Parameters:
          castle_id : int
            id of castle
          itemname : byte string
            name the item want to check
        :Return: True if the new one is better or "you_had_best" if the old the still good
        """
        for castle in self.castles:
            if castle.id == castle_id:
                if castle.defence == 0:
                    return True
                else:
                    if ITEM['defence'][itemname]['value'] > castle.defence:
                        return True
                    else: return "you_had_best"

    def buy_defense(self,team_id, castle_id, itemname):
        """Buy a defence item and set for castle.

        :Parameters:
          team_id : int
            The id of the team 
          itemname : byte string
            name the item want to buy  
          castle_id : int
            The id of the castle 
        :Return: Boolean or byte string (if it had error)
        """
        # Find the castle
        for castle in self.castles:
            if castle.id == castle_id:
            	# You can do this if you own this castle
                if castle.owner_id == team_id:
                    result = self.check_item_defence(castle_id, itemname)
                    # You can buy the new one if it better
                    if result != True: return result
                    else:
                        result = self.buy_item(team_id, itemname, 'defence')
                        if result != True:
                            return result
                        else:
                            castle.defence = ITEM['defence'][itemname]['value']
                            return True
                else:
                    return "not_owner"
        # Find the Team        
        for team in self.teams:
            if team.id == team_id:
                if not team.is_have(itemname):
                    if self.buy_item(team_id, itemname,'defence') != True:
                        return "cant_set_defense"

                for castle in self.castles:
                    if castle.id == castle_id:
                    	# After buy, use it on a own castle
                        if team.use_item(itemname):
                            if castle.change_def(itemname):
                                return True
                            else :
                                return False
                return "not_found_castle"
        return "not_found_team"

    def get_question_mine(self, mine_id, resource):
        """Get content and 4 choices of question of that resource.

        :Parameters:
          mine_id : int
            id of field
          resource : byte string
            A field has 3 resource with 3 question, choose one of them
        :Return: byte string (content, [choice1, choice2,...])
        """
        Id = self.get_questionid_mine(mine_id, resource)
        if Id == False:
            return None, None
        else:
            return self.get_question_by_id(Id)

    
    def check_answer_mine(self, mine_id, resource, team_id, answer):
        """Check the answer of the question of resoure.

        :Parameters:
          answer : byte string
            answer string ('1','2','3','4')
          question_id : int
            The id of the question 
          mine_id : int
            id of field
          resource : byte string
            A field has 3 resource with 3 question, choose one of them

        :Return: Boolean or byte string (if it had error)
        """

        #Check if that question is solved by our team
        if self.check_question_mine(mine_id, resource, team_id):
            return "is_solved"
        else:
            quesId = self.get_questionid_mine(mine_id, resource)
            if self.check_answer(answer, quesId):
                for i in self.fields:
                    if i.id == mine_id:
                        i.add_solver(resource, team_id)
                        return True
                return False
            else:
                return False

    def check_castle(self, team_id, castle_id):
        """Get status of a castle.

        :Parameters:
          team_id : int
            The id of the team 
          itemname : byte string
            name the item want to buy  
          castle_id : int
            The id of the castle
        :Return: byte string 
        """
        for team in self.teams:
            if team.id == team_id:
                for castle in self.castles:
                    if castle.id == castle_id:
                        print ('1: {}'.format(castle.is_blocked))
                        if castle.is_blocked:
                            return "blocked"
                        elif castle.owner_id == None:
                            castle.set_block()
                            print ('2: {}'.format(castle.is_blocked))
                            return "empty_castle"
                        elif team_id == castle.owner_id:
                            return "our_castle"
                        else :
                            return True
                return "not_found_castle"
        return "not_found_team"

    def attack_castle(self, team_id, castle_id):
        """Attack the castle.

        :Parameters:
          team_id : int
            The id of the team 
          
          castle_id : int
            The id of the castle 
        :Return: Boolean / byte string (if get error) 
        """
        for team in self.teams:
            if team.id == team_id:
                for castle in self.castles:
                    if castle.id == castle_id:
                        castle.set_block()
                        try:
                            itemattack = team.inventory[0]
                        except:
                            itemattack = None
                        try:
                            if castle.is_attack_success(itemattack):
                                castle.block_time += 60*5
                                castle.attacked(team_id)
                                return True
                            else:
                                castle.remove_block()
                                team.inventory = []
                                return False
                        except:
                            return "Error"
                return "not_found_castle"
        return "not_found_team"


    def team_attack(self, team_id, castle_id):
        """team attack castle
        Check if the castle is blocked or not. And call func to attack it
        :Parameters:
          team_id : int
            The id of the team 
          
          castle_id : int
            The id of the castle 
        :Return: Boolean / byte string 
        """
        result = self.check_castle(team_id, castle_id)
        if result != True:
            return result
        else:
            return self.attack_castle(team_id, castle_id)


    def check_weapon_attack(self, team_id, weapon):
        for i in self.teams():
            if i.id == team_id:
                return weapon in i.inventory and weapon in ITEM['attack'].keys()
        return "not_found_team"


    def get_questionid_castle(self, castle_id):
        """Get question content of resource in a field.

        :Parameters:
          castle_id : int
            id of castle
            
        :Return: int (question_id)
        """
        for castle in self.castles:
            if castle.id == castle_id:
                return castle.question_id
        return "not_found_castle"

    def check_answer_castle(self, castle_id, answer):
        """Check answer of castle's question
        

        :Parameters:
          answer : byte string
            answer string ('1','2','3','4')
          question_id : int
            The id of the question 
          castle_id : int
          	the id of castle
        :Return: Boolean or byte string (if it had error)
        """
        quesId = self.get_questionid_castle(castle_id)
        result = self.check_answer(answer, quesId)
        if result == True:
            return True
        else:
            for castle in self.castles:
                if castle.id == castle_id:
                    castle.remove_block()
                    return result
            return False

    def answer_castles_success(self, team_id, castle_id):
        """Change status of the castle after break through challenge question.

        :Parameters:
          team_id : int
            The id of the team 
          
          castle_id : int
            The id of the castle 
        :Return: byte string
        """
        for team in self.teams:
            if team.id == team_id:
                for castle in self.castles:
                    if castle.id == castle_id:
                        castle.change_owner(team_id)
                        castle.change_question()
                return "not_found_castle"
        return "not_found_castle"


    def getData(self):
        """Get info of a team: id, name, resource, inventory
        :Parameters:
          
        :Return: byte string
        """
        list_team = []
        for i in self.teams:
            list_team.append((i.id, i.resources["gold"], i.resources["iron"], i.resources["wood"], i.resources["stone"]))
        list_defend = []
        list_owner = []
        for castle in self.castles:
            list_defend.append(castle.defence)
            list_owner.append(castle.owner_id)
        return list_team, list_defend, list_owner

    def add_resource(self, team_id):
    	"""add resource for a team: like cheat in AoE
        :Parameters:
          team_id : int
            The id of the team 
        :Return: None
        """
        for team in self.teams:
            if team.id == team_id:
                team.add_resource("gold", 10000)
                team.add_resource("iron", 10000)
                team.add_resource("stone", 10000)
                team.add_resource("wood", 10000)



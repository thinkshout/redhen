<?php

/**
 * Redhen Contact Type UI controller.
 */
class RedhenContactTypeUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
    $items[$this->path]['description'] = 'Manage contact entity types, including adding and removing fields and the display of fields.';
    return $items;
  }

  /**
   * Override parent::operationForm to set a more meaningful message on delete.
   */
  public function operationForm($form, &$form_state, $entity, $op) {
    switch ($op) {
      case 'delete':
        $label = entity_label($this->entityType, $entity);
        $confirm_question = t('Are you sure you want to delete the %entity %label? ', array('%entity' => $this->entityInfo['label'], '%label' => $label));
        $desc = t('This action will also delete <strong>all</strong> contacts of type %label and cannot be undone.', array('%entity' => $this->entityInfo['label'], '%label' => $label));
        return confirm_form($form, $confirm_question, $this->path, $desc);

      default:
        return parent::operationForm($form, $form_state, $entity, $op);
    }
  }

}

<?php
/**
 * @file
 * Redhen Contact entity class
 */

/**
 * The class used for contact entities.
 */
class RedhenContact extends Entity {

  public function __construct(array $values = array(), $entityType = NULL) {
    parent::__construct($values, $entityType);
  }

  /**
   * Override buildContent() to add contact properties.
   */
  public function buildContent($view_mode = 'full', $langcode = NULL) {
    $content['name'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Name'),
      '#items' => array(
        array(
          '#markup' => check_plain($this->first_name) . ' ' . check_plain($this->last_name)
        ),
       ),
      '#classes' => 'field field-label-inline clearfix',
    );

    return entity_get_controller($this->entityType)
			->buildContent($this, $view_mode, $langcode, $content);
  }

  /**
   * Specifies the default label, which is picked up by label() by default.
   */
  protected function defaultLabel() {
    $type = redhen_contact_get_types($this->type);
    return $type->label . ': ' . $this->first_name . ' ' . $this->last_name;
  }

}

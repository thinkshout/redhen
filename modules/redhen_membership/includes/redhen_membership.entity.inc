<?php
/**
 * @file
 * Redhen Group entity classes
 */

/**
 * The class used for group entities.
 */
class RedhenMembership extends Entity {

  public $org;

  public function __construct(array $values = array()) {
    parent::__construct($values, 'redhen_membership');

    $this->org = $this->organization();
  }

  /**
   * Override buildContent() to add contact properties.
   */
  public function buildContent($view_mode = 'full', $langcode = NULL) {
    $content['org'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Organization'),
      '#items' => array(
        array(
          '#markup' => check_plain($this->org->label)
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
    );
    $content['name'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('Label'),
      '#items' => array(
        array(
          '#markup' => check_plain($this->label)
        ),
      ),
      '#classes' => 'field field-label-inline clearfix',
    );
    $content['redhen_state'] = array(
      '#theme' => 'redhen_property_field',
      '#label' => t('State'),
      '#items' => array(
        array(
          '#markup' => ($this->redhen_state == REDHEN_STATE_ACTIVE) ? t('Active') : t('Inactive')
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
    $type = redhen_membership_get_types($this->type);
    return $type->label . ': ' . $this->label;
  }

  /**
   * Specifify URI
   */
  protected function defaultUri() {
    return array('path' => 'redhen/org/' . $this->org_id . '/memberships/' . $this->identifier());
  }

  /**
   * Return an organization object if possible.
   *
   * @return RedhenOrg|null
   */
  protected function organization() {
    $org = NULL;
    if (isset($this->org_id)) {
      $org = redhen_org_load($this->org_id);
    }
    return $org;
  }

}

/**
 * Controls metadata for Redhen memberships
 */
class RedhenMembershipMetadataController extends EntityDefaultMetadataController {
  public function entityPropertyInfo() {
    $info = parent::entityPropertyInfo();
    $properties = &$info[$this->type]['properties'];

    $properties['created'] = array(
      'label' => t("Created"),
      'description' => t("The date the membership was created."),
      'type' => 'date',
      'schema field' => 'created',
    );
    $properties['updated'] = array(
      'label' => t("Updated"),
      'description' => t("The date the membership was updated."),
      'type' => 'date',
      'schema field' => 'updated',
    );
    $properties['author'] = array(
      'label' => t("Author"),
      'type' => 'user',
      'description' => t("The author of the membership record."),
      'schema field' => 'author_uid'
    );

    return $info;
  }
}

